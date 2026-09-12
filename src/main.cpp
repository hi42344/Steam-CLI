#include <iostream>
#include <string>
#include <vector>
#include "steam_win32.hpp"
#include "vdf_parser.hpp"

struct CLI_ERROR : std::runtime_error {
    explicit CLI_ERROR(const std::string& msg)
        : std::runtime_error("Error: " + msg) {}

    explicit CLI_ERROR(const char* msg)
        : std::runtime_error("Error: " + std::string(msg)) {}
};

int main(int argc, char* argv[]) {
    try {
        if (argc == 1) {
            throw CLI_ERROR("No command");
        }

        std::string command = argv[1];

        if (command == "run") {
            if (argc == 2) {
                throw CLI_ERROR("Missing game name");
            }

            // Combine all remaining arguments (if any) into a single name string
            std::string game_name = argv[2];
            if (argc > 3) {
                for (int i = 3; i < argc; i++) {
                    game_name += " ";
                    game_name += argv[i];
                }
            }

            // Locate Steam directory
            std::string steam_path = steam::get_steam_path();
            if (steam_path.empty()) {
                throw CLI_ERROR("Could not locate Steam installation");
            }

            // Scan installed games in steamapps
            auto library_paths = steam::get_library_paths(steam_path);
            auto installed_games = steam::scan_installed_games(library_paths);

            // Match game name to AppID
            std::string app_id = steam::find_appid_by_name(installed_games, game_name);
            if (app_id.empty()) {
                throw CLI_ERROR("Could not find game matching \"" + game_name + "\"");
            }

            // Launch the game
            std::cout << "Launching " << game_name << "...";
            steam::launch_game(app_id);
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