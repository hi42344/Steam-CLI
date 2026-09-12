#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>

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

            if (name_lower.find(target_lower) != std::string::npos) {
                return &game;
            }
        }
        return nullptr;
    }

    inline std::string find_appid_by_name(std::vector<GameInfo>& games, const std::string& target_name) {
        GameInfo* game = find_game_by_name(games, target_name);
        return game ? game->app_id : "";
    }

}