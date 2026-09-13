#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include "steam_win32.hpp"
#include "vdf_parser.hpp"

struct CLI_ERROR : std::runtime_error {
    explicit CLI_ERROR(const std::string& msg)
        : std::runtime_error("Error: " + msg) {}

    explicit CLI_ERROR(const char* msg)
        : std::runtime_error("Error: " + std::string(msg)) {}
};

void print_game_info(const steam::GameInfo& game) {
    double size_gb = static_cast<double>(game.size_bytes) / (1024.0 * 1024.0 * 1024.0);
    std::filesystem::path full_path = game.library_path / "common" / game.install_dir;

    std::cout << "Name:         " << game.name << "\n";
    std::cout << "AppID:        " << game.app_id << "\n";
    std::cout << "Size on Disk: " << std::defaultfloat << size_gb << " GB\n";
    std::cout << "Path:         " << full_path.string() << "\n\n";
}

bool is_number(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

int main(int argc, char* argv[]) {
    try {
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

        //The seperator for when printing games/game infos
        auto game_sep = "-------------";
        std::string command = argv[1];

        // Locate Steam directory and scan libraries
        std::string steam_path = steam::get_steam_path();
        if (steam_path.empty()) {
            throw CLI_ERROR("Could not locate Steam installation");
        }

        //Get library paths and installed games from steamapps and other drives
        auto library_paths = steam::get_all_library_paths(steam_path);
        auto installed_games = steam::scan_installed_games(library_paths);

        //'steam run GAME_NAME' Runs a game
        if (command == "run") {
            //'steam run' with no game name
            if (argc == 2) {
                throw CLI_ERROR("Missing game name");
            }

            std::string game_name = argv[2];
            combine_args(game_name);

            //Game name -> App id, or error if not found
            std::string app_id = steam::find_appid_by_name(installed_games, game_name);
            if (app_id.empty()) {
                throw CLI_ERROR("Could not find game matching \"" + game_name + "\"");
            }

            std::cout << "Launching " << game_name << "...";
            steam::launch_game(app_id);
        }
        //'steam verify GAME_NAME' Triggers file validation for a game
        else if (command == "verify") {
            if (argc == 2) {
                throw CLI_ERROR("Missing game name");
            }

            std::string game_name = argv[2];
            combine_args(game_name);

            std::string app_id = steam::find_appid_by_name(installed_games, game_name);
            if (app_id.empty()) {
                throw CLI_ERROR("Could not find game matching \"" + game_name + "\"");
            }

            std::cout << "Verifying files for " << game_name << "...";
            steam::verify_game_files(app_id);
        }
        //'steam install GAME NAME/APPID' Prompts Steam to install a game
        else if (command == "install") {
            if (argc == 2) {
                throw CLI_ERROR("Missing game name or AppID");
            }

            std::string arg = argv[2];
            combine_args(arg);

            std::string app_id;

            // 1. Direct AppID passed
            if (is_number(arg)) {
                app_id = arg;
            }
            else {
                // 2. Check installed games locally first
                app_id = steam::find_appid_by_name(installed_games, arg);

                // 3. Prompt user from online search results if not found locally
                if (app_id.empty()) {
                    app_id = steam::search_appid_online(arg);
                }
            }

            //Other type of error
            if (app_id.empty()) {
                throw CLI_ERROR("Installation cancelled");
            }
            //If not found we will error that
            if (app_id == steam::SEARCH_RESULT_NOT_FOUND) {
                throw CLI_ERROR("\"" + arg + "\" was not found");
            }

            std::cout << "Installing " << app_id << "...\n";
            steam::install_game(app_id);
        }
        //'steam uninstall GAME_NAME' Prompts Steam to uninstall a game
        else if (command == "uninstall") {
            if (argc == 2) {
                throw CLI_ERROR("Missing game name");
            }

            std::string game_name = argv[2];
            combine_args(game_name);

            std::string app_id = steam::find_appid_by_name(installed_games, game_name);
            if (app_id.empty()) {
                throw CLI_ERROR("Could not find game matching \"" + game_name + "\"");
            }

            std::cout << "Uninstalling " << game_name << "...";
            steam::uninstall_game(app_id);
        }
        //'steam list' Prints all installed, game names are seperated
        else if (command == "list") {
            if (installed_games.empty()) {
                std::cout << "No installed Steam games found.";
                return 0;
            }

            std::cout << game_sep << "\n";
            for (const auto& game : installed_games) {
                std::cout << game.name << "\n" << game_sep << "\n";
            }
        }
        //'steam info GAME_NAME/--all' shows info about a game or all games, if --all game names are seperated
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

                std::cout << "Installed Games (" << installed_games.size() << ")\n" << game_sep << '\n';
                for (const auto& game : installed_games) {
                    print_game_info(game);
                    std::cout << game_sep << '\n';
                }
            }
            else {
                steam::GameInfo* game = steam::find_game_by_name(installed_games, arg);
                if (!game) {
                    throw CLI_ERROR("Could not find game matching \"" + arg + "\"");
                }

                print_game_info(*game);
            }
        }
        else {
            throw CLI_ERROR("Unknown command: " + command);
        }
    }
    catch (const CLI_ERROR& e) {
        std::cerr << e.what();
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what();
        return 1;
    }
    catch (const char* msg) {
        std::cerr << "Error: " << msg;
        return 1;
    }
    catch (...) {
        std::cerr << "Error: unknown exception";
        return 1;
    }

    return 0;
}