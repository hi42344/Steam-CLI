#pragma once
#pragma comment(lib, "wininet.lib")
#include <windows.h>
#include <wininet.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <filesystem>
#include <string_view>
#include <boost/json/src.hpp>

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

    struct SearchResult {
        std::string id;
        std::string name;
        std::string type;
    };

    // Fetches online search hits from Steam without forcing interactive selection
    inline std::vector<SearchResult> get_results_online(const std::string& game_name) {
        std::vector<SearchResult> results;

        HINTERNET hInternet = ::InternetOpenA("steam-cli", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
        if (!hInternet) return results;

        std::string query = game_name;
        size_t pos = 0;
        while ((pos = query.find(' ', pos)) != std::string::npos) {
            query.replace(pos, 1, "%20");
            pos += 3;
        }

        std::string url = "https://store.steampowered.com/api/storesearch/?term=" + query + "&l=english&cc=US";

        HINTERNET hConnect = ::InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (!hConnect) {
            ::InternetCloseHandle(hInternet);
            return results;
        }

        std::string response;
        char buffer[4096];
        DWORD bytesRead = 0;

        while (::InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            response += buffer;
        }

        ::InternetCloseHandle(hConnect);
        ::InternetCloseHandle(hInternet);

        if (response.empty()) return results;

        try {
            boost::json::value jv = boost::json::parse(response);
            const auto& obj = jv.as_object();

            if (obj.contains("items")) {
                const auto& items = obj.at("items").as_array();
                for (const auto& item_val : items) {
                    const auto& item = item_val.as_object();

                    std::string id = std::to_string(item.at("id").as_int64());
                    std::string name = item.contains("name") ? std::string(item.at("name").as_string()) : "Unknown";
                    std::string type = item.contains("type") ? std::string(item.at("type").as_string()) : "app";

                    bool exists = false;
                    for (const auto& res : results) {
                        if (res.id == id) {
                            exists = true;
                            break;
                        }
                    }

                    if (!exists) {
                        results.push_back({ id, name, type });
                    }
                }
            }
        }
        catch (...) {
            return results;
        }

        return results;
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

        std::cout << "\nSelect target (1-" << results.size() << "):\n";
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "  [" << (i + 1) << "] " << results[i].name
                << " (" << results[i].type << ", AppID: " << results[i].id << ")\n";
        }
        std::cout << "> ";

        int choice = 0;
        if (std::cin >> choice && choice > 0 && choice <= static_cast<int>(results.size())) {
            return results[choice - 1].id;
        }

        std::cin.clear();
        return results[0].id;
    }

}