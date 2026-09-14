// Copyright (C) 2026 idk1234
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include "steam_win32.hpp"
#include "vdf_parser.hpp"
#include "Custom_commands.hpp"
#include "helpers/Colors.hpp"
#include "helpers/Enviorment_vars.hpp"

struct CLI_ERROR : std::runtime_error {
    explicit CLI_ERROR(const std::string& msg)
        : std::runtime_error(std::string(color::error) + "Error: " + msg + color::reset) {}

    explicit CLI_ERROR(const char* msg)
        : std::runtime_error(std::string(color::error) + "Error: " + std::string(msg) + color::reset) {}
};

void print_game_info(const steam::GameInfo& game) {
    double size_gb = static_cast<double>(game.size_bytes) / (1024.0 * 1024.0 * 1024.0);
    std::filesystem::path full_path = game.library_path / "common" / game.install_dir;

    std::cout << "Name:         " << game.name << "\n";
    std::cout << "AppID:        " << game.app_id << "\n";
    std::cout << "Size on Disk: " << std::defaultfloat << size_gb << " GB\n";
    std::cout << "Path:         " << full_path.string() << "\n\n";
}

void print_search_info(const steam::SearchResult& search_res) {
    std::cout << "| Name:  " << search_res.name << "\n";
    std::cout << "| AppID: " << search_res.id << "\n";
    std::cout << "| Type:  " << search_res.type << "\n";
}

bool is_number(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

inline void app_id_not_found(const std::string& game_name) {
    throw CLI_ERROR("Could not find game matching \"" + game_name + "\"");
}

int main(int argc, char* argv[]) {
    constexpr const char* MISSING_GAME_NAME_ERROR = "Missing game name";
    constexpr const char* WARNING_ = "Warning: ";
    //The seperator for when printing games/game infos
    constexpr const char* GAME_SEP = "-------------";
    try {
        auto exe_directory = steam::get_exe_directory();
        //Add to PATH enviorment variable if not already there
        if (!env_vars::path::has(exe_directory.string())) {
            env_vars::path::append_permanent(exe_directory.string());
            std::cout << color::bold << "Added " << exe_directory << " to PATH enviorment variable" << color::reset;
            return 0;
        }

        if (argc == 1) {
            throw CLI_ERROR("No command provided");
        }

        //Combine remaining args for game name
        auto combine_args = [argc, argv](std::string& game_name_) {
            for (int i = 3; i < argc; i++) {
                game_name_ += " ";
                game_name_ += argv[i];
            }
            };

        std::string command = argv[1];

        // Locate Steam directory and scan libraries
        std::string steam_path = steam::get_steam_path();
        if (steam_path.empty()) {
            throw CLI_ERROR("Could not locate Steam installation");
        }

        //Get library paths and installed games from steamapps and other drives
        auto library_paths = steam::get_all_library_paths(steam_path);
        auto installed_games = steam::scan_installed_games(library_paths);

        //Getting either EXE_PATH/config/Commands.json or EXE_PATH_parent/config/Commands.json, or if not either one of those, its steam::COMMANDS_JSON_NOT_FOUND
        std::filesystem::path config_path = exe_directory / "config" / "Commands.json";

        if (!std::filesystem::exists(config_path)) {
            config_path = exe_directory.parent_path() / "config" / "Commands.json";
        }

        std::unordered_map<std::string, steam::CustomCommand> custom_commands;

        if (std::filesystem::exists(config_path)) {
            custom_commands = steam::load_custom_commands(config_path.string());
        }
        else {
            std::cerr << color::warning << WARNING_ << "Could not find config/Commands.json, Custom commands will not be available" << color::reset << "\n";
            config_path = steam::COMMANDS_JSON_NOT_FOUND;
        }

        // 'steam list' Prints all installed games
        if (command == "list") {
            if (installed_games.empty()) {
                std::cout << "No installed Steam games found.";
                return 0;
            }

            std::cout << GAME_SEP << "\n";
            for (const auto& game : installed_games) {
                std::cout << game.name << "\n" << GAME_SEP << "\n";
            }
        }
        // 'steam info GAME_NAME/--all'
        else if (command == "info") {
            if (argc == 2) {
                throw CLI_ERROR("Missing argument for 'info' (provide a game name or '--all')");
            }

            std::string arg = argv[2];
            combine_args(arg);

            if (arg == "--all") {
                if (installed_games.empty()) {
                    std::cout << "No installed Steam games found.\n";
                    return 0;
                }

                std::cout << "Installed Games (" << installed_games.size() << ")\n" << GAME_SEP << '\n';
                for (const auto& game : installed_games) {
                    print_game_info(game);
                    std::cout << GAME_SEP << '\n';
                }
            }
            else {
                steam::GameInfo* game = steam::find_game_by_name(installed_games, arg);
                if (!game) {
                    app_id_not_found(arg);
                }

                print_game_info(*game);
            }
        }
        // 'steam search QUERY'
        else if (command == "search") {
            if (argc == 2) {
                throw CLI_ERROR("Missing search query");
            }

            std::string query = argv[2];
            combine_args(query);

            auto results = steam::get_results_online(query);

            if (results.empty()) {
                throw CLI_ERROR("No games found matching \"" + query + "\"");
            }

            std::cout << '|' << GAME_SEP << "\n";
            for (const auto& res : results) {
                print_search_info(res);
                std::cout << "|\n|" << GAME_SEP << '\n';
            }
        }
        // 'steam path GAME NAME' -> Opens Steam folder, or game folder if passed
        else if (command == "path") {
            if (argc == 2) {
                std::cout << steam_path;
                steam::open_folder(steam_path);
            }
            else {
                std::string game_name = argv[2];
                combine_args(game_name);

                steam::GameInfo* game = steam::find_game_by_name(installed_games, game_name);
                if (!game) {
                    app_id_not_found(game_name);
                }

                std::filesystem::path game_path = game->library_path / "common" / game->install_dir;
                std::cout << game_path.string();
                steam::open_folder(game_path);
            }
        }
        else if (command == "config") {
            if (argc == 2) {
                throw CLI_ERROR("Missing sub-command in config command");
            }

            if (config_path == steam::COMMANDS_JSON_NOT_FOUND) {
                throw CLI_ERROR("config/Commands.json does not exist, can not use a sub-command with it");
            }

            std::string sub_cmd = argv[2];

            if (sub_cmd == "open") {
                std::cout << "Opening Commands.json in default text editor...";
                steam::open_in_default_editor(config_path);
            }
            else {
                throw CLI_ERROR("Unknown sub-command \"" + sub_cmd + "\"");
            }
        }
        // Fallback to custom command lookup
        else {
            auto it = custom_commands.find(command);
            if (it != custom_commands.end()) {
                const auto& custom_cmd = it->second;
                std::string arg;

                if (custom_cmd.needs_arg) {
                    if (argc == 2) {
                        throw CLI_ERROR(MISSING_GAME_NAME_ERROR);
                    }
                    arg = argv[2];
                    combine_args(arg);
                }

                bool success = steam::execute_custom_command(custom_cmd, arg, installed_games);
                if (!success && custom_cmd.needs_arg) {
                    app_id_not_found(arg);
                }
            }
            else {
                throw CLI_ERROR("Unknown command \"" + command + "\"");
            }
        }
    }
    catch (const CLI_ERROR& e) {
        std::cerr << e.what();
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << color::error << "Error: " << e.what();
        return 1;
    }
    catch (const char* msg) {
        std::cerr << color::error << "Error: " << msg;
        return 1;
    }
    catch (...) {
        std::cerr << color::error << "Error: unknown exception";
        return 1;
    }

    return 0;
}