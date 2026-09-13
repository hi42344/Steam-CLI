#pragma once
#include <windows.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <filesystem>

namespace steam {

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

    inline void launch_game(const std::string& app_id) {
        std::string uri = "steam://run/" + app_id;
        ShellExecuteA(NULL, "open", uri.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }

    inline void verify_game_files(const std::string& app_id) {
        std::string uri = "steam://validate/" + app_id;
        ShellExecuteA(NULL, "open", uri.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }

    inline void uninstall_game(const std::string& app_id) {
        std::string uri = "steam://uninstall/" + app_id;
        ShellExecuteA(NULL, "open", uri.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }

}