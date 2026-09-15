// ============================================================================
// License: MIT License (Permissive - free to use in any project)
//
// Copyright (c) 2026 hi42344
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ============================================================================

#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <boost/json/src.hpp>

namespace fs = std::filesystem;

namespace steam {

    struct GameInfo {
        std::string app_id;
        std::string name;
        std::string install_dir;
        uint64_t size_bytes = 0;
        fs::path library_path;
    };

    inline std::vector<fs::path> get_all_library_paths(const std::string& steam_path) {
        std::vector<fs::path> library_paths;

        fs::path main_apps = fs::path(steam_path) / "steamapps";
        if (fs::exists(main_apps)) {
            library_paths.push_back(main_apps);
        }

        fs::path vdf_path = main_apps / "libraryfolders.vdf";
        if (fs::exists(vdf_path)) {
            std::ifstream file(vdf_path);
            std::string line;

            while (std::getline(file, line)) {
                size_t path_key = line.find("\"path\"");
                if (path_key != std::string::npos) {
                    size_t first_quote = line.find('"', path_key + 6);
                    size_t second_quote = line.find('"', first_quote + 1);

                    if (first_quote != std::string::npos && second_quote != std::string::npos) {
                        std::string raw_path = line.substr(first_quote + 1, second_quote - first_quote - 1);

                        std::string clean_path;
                        for (size_t i = 0; i < raw_path.length(); ++i) {
                            if (raw_path[i] == '\\' && i + 1 < raw_path.length() && raw_path[i + 1] == '\\') {
                                clean_path += '\\';
                                ++i;
                            }
                            else {
                                clean_path += raw_path[i];
                            }
                        }

                        fs::path extra_apps = fs::path(clean_path) / "steamapps";
                        if (fs::exists(extra_apps) && std::find(library_paths.begin(), library_paths.end(), extra_apps) == library_paths.end()) {
                            library_paths.push_back(extra_apps);
                        }
                    }
                }
            }
        }

        return library_paths;
    }

    inline std::vector<GameInfo> scan_installed_games(const std::vector<fs::path>& library_paths) {
        std::vector<GameInfo> games;

        for (const auto& lib_path : library_paths) {
            if (!fs::exists(lib_path)) continue;

            for (const auto& entry : fs::directory_iterator(lib_path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".acf") {
                    std::ifstream file(entry.path());
                    std::string line;
                    GameInfo info;
                    info.library_path = lib_path;

                    while (std::getline(file, line)) {
                        auto extract_val = [&](const std::string& key) -> std::string {
                            if (line.find("\"" + key + "\"") != std::string::npos) {
                                size_t last_quote = line.rfind('"');
                                size_t prev_quote = line.rfind('"', last_quote - 1);
                                if (prev_quote != std::string::npos && last_quote > prev_quote) {
                                    return line.substr(prev_quote + 1, last_quote - prev_quote - 1);
                                }
                            }
                            return "";
                            };

                        if (info.app_id.empty()) info.app_id = extract_val("appid");
                        if (info.name.empty()) info.name = extract_val("name");
                        if (info.install_dir.empty()) info.install_dir = extract_val("installdir");

                        if (info.size_bytes == 0) {
                            std::string size_str = extract_val("SizeOnDisk");
                            if (!size_str.empty()) {
                                info.size_bytes = std::stoull(size_str);
                            }
                        }
                    }

                    if (!info.app_id.empty() && !info.name.empty()) {
                        games.push_back(info);
                    }
                }
            }
        }

        return games;
    }

    inline GameInfo* find_game_by_name(std::vector<GameInfo>& games, const std::string& target_name) {
        std::string target_lower = target_name;
        std::transform(target_lower.begin(), target_lower.end(), target_lower.begin(), ::tolower);

        for (auto& game : games) {
            std::string name_lower = game.name;
            std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);

            // Changed to a stricter version since adding the spell checker negates what help this would do
            if (name_lower == target_lower) {
                return &game;
            }
        }
        return nullptr;
    }

    inline std::string find_appid_by_name(std::vector<GameInfo>& games, const std::string& target_name) {
        GameInfo* game = find_game_by_name(games, target_name);
        return game ? game->app_id : "";
    }

    struct SearchResult {
        std::string id;
        std::string name;
        std::string type;
    };

    // Fetches online search hits from Steam
    inline std::vector<SearchResult> get_results_online(const std::string& game_name, const char* identifier_for_internetopena = "steam-cli") {
        std::vector<SearchResult> results;

        HINTERNET hInternet = ::InternetOpenA(identifier_for_internetopena, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
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

}