#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>

namespace steam {

    struct GameInfo {
        std::string app_id;
        std::string name;
    };

    inline std::vector<GameInfo> scan_installed_games(const std::vector<std::filesystem::path>& library_paths) {
        std::vector<GameInfo> games;

        for (const auto& lib_path : library_paths) {
            if (!std::filesystem::exists(lib_path)) continue;

            for (const auto& entry : std::filesystem::directory_iterator(lib_path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".acf") {
                    std::ifstream file(entry.path());
                    std::string line;
                    GameInfo info;

                    while (std::getline(file, line)) {
                        if (info.app_id.empty() && line.find("\"appid\"") != std::string::npos) {
                            size_t last_quote = line.rfind('"');
                            size_t prev_quote = line.rfind('"', last_quote - 1);
                            if (prev_quote != std::string::npos) {
                                info.app_id = line.substr(prev_quote + 1, last_quote - prev_quote - 1);
                            }
                        }
                        if (info.name.empty() && line.find("\"name\"") != std::string::npos) {
                            size_t last_quote = line.rfind('"');
                            size_t prev_quote = line.rfind('"', last_quote - 1);
                            if (prev_quote != std::string::npos) {
                                info.name = line.substr(prev_quote + 1, last_quote - prev_quote - 1);
                            }
                        }
                        if (!info.app_id.empty() && !info.name.empty()) break;
                    }

                    if (!info.app_id.empty() && !info.name.empty()) {
                        games.push_back(info);
                    }
                }
            }
        }

        return games;
    }

    inline std::string find_appid_by_name(const std::vector<GameInfo>& games, const std::string& target_name) {
        std::string target_lower = target_name;
        std::transform(target_lower.begin(), target_lower.end(), target_lower.begin(), ::tolower);

        for (const auto& game : games) {
            std::string name_lower = game.name;
            std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);

            if (name_lower.find(target_lower) != std::string::npos) {
                return game.app_id;
            }
        }
        return "";
    }

}