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

#pragma once
#pragma comment(lib, "wininet.lib")
#include <windows.h>
#include <wininet.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <filesystem>
#include <string_view>
#include "vdf_parser.hpp"
#include "helpers/Colors.hpp"

namespace steam {
    constexpr std::string_view SEARCH_RESULT_NOT_FOUND = "<[UNOFFICAL_STEAM_CLI: NO GAME FOUND]>";
    constexpr std::string_view COMMANDS_JSON_NOT_FOUND = "<[UNOFFICAL_STEAM_CLI: COMMANDS_JSON_NOT_FOUND]>";

    inline std::string get_steam_path() {
        HKEY hKey;
        char path[MAX_PATH];
        DWORD bufSize = sizeof(path);

        if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Valve\\Steam", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            if (RegQueryValueExA(hKey, "SteamPath", NULL, NULL, (LPBYTE)path, &bufSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return std::string(path);
            }
            RegCloseKey(hKey);
        }
        return "";
    }

    inline std::vector<std::filesystem::path> get_library_paths(const std::string& steam_path) {
        std::vector<std::filesystem::path> paths;
        std::filesystem::path main_apps = std::filesystem::path(steam_path) / "steamapps";

        if (std::filesystem::exists(main_apps)) {
            paths.push_back(main_apps);
        }

        return paths;
    }

    //So we can use relative paths when possible
    inline std::filesystem::path get_exe_directory() {
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        return std::filesystem::path(path).parent_path();
    }

    inline void execute_uri(const std::string& raw_uri) {
        std::string uri = raw_uri;
        // Prefix steam:// if the user provided uri doesn't have a prefix of http/https/steam://
        if (uri.rfind("steam://", 0) != 0 && uri.rfind("https://", 0) != 0 && uri.rfind("http://", 0) != 0) {
            uri = "steam://" + uri;
        }
        ShellExecuteA(NULL, "open", uri.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }

    // Opens the specified filesystem folder in Windows Explorer
    inline void open_folder(const std::filesystem::path& folder_path) {
        ShellExecuteA(NULL, "open", folder_path.string().c_str(), NULL, NULL, SW_SHOWNORMAL);
    }

    inline void open_in_default_editor(const std::filesystem::path& filePath) {
        ShellExecuteA(
            NULL,           // Parent window handle
            "open",         // Operation / verb
            filePath.string().c_str(),       // File path
            NULL,           // Parameters
            NULL,           // Working directory
            SW_SHOWNORMAL   // Show window flag
        );
    }

    // Wraps get_results_online for interactive flows like `steam install`
    inline std::string search_appid_online(const std::string& game_name) {
        auto results = get_results_online(game_name);

        if (results.empty()) {
            return SEARCH_RESULT_NOT_FOUND.data();
        }

        if (results.size() == 1) {
            return results[0].id;
        }

        std::cout << "\nSelect target (1-" << results.size() << ", x/X to not choose any):\n";
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "  [" << (i + 1) << "] " << results[i].name
                << " (type: " << results[i].type << ", AppID: " << results[i].id << ")\n";
        }
        std::cout << "> ";

        std::string input;
        if (std::cin >> input) {
            // Check if user wants to exit
            if (input == "x" || input == "X") {
                std::cout << color::error << "Exiting" << color::reset;
                std::exit(0);
                return "";
            }

            try {
                // Convert string to integer
                int choice = std::stoi(input);

                // Check 1-based bounds against results vector
                if (choice > 0 && choice <= static_cast<int>(results.size())) {
                    return results[choice - 1].id;
                }
            }
            catch (const std::invalid_argument&) {
                throw std::runtime_error("Choice must be a valid integer");
            }
            catch (const std::out_of_range&) {
                throw std::runtime_error("Choice must be within the size of a 32-bit integer");
            }
            catch (...) {
                throw std::runtime_error("Error in search app id");
            }
        }

        std::cin.clear();
        return results[0].id;
    }

}