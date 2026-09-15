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
#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include "steam_win32.hpp"
#include "vdf_parser.hpp"
#include "helpers/Misc.hpp"
#include "Spell_checker.hpp"

namespace steam {

    struct CustomCommand {
        std::string uri;
        bool local = false;
        bool online = false;
        std::string print_message;
        bool needs_arg = true;
    };

    // Helper to replace all occurrences of a placeholder in a string
    inline void replace_all(std::string& str, const std::string& from, const std::string& to) {
        if (from.empty()) return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }

    // Loads custom command definitions from JSON
    inline std::unordered_map<std::string, CustomCommand> load_custom_commands(const std::filesystem::path& json_path) {
        std::unordered_map<std::string, CustomCommand> custom_cmds;

        if (!std::filesystem::exists(json_path)) return custom_cmds;

        std::ifstream file(json_path);
        if (!file.is_open()) return custom_cmds;

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (content.empty()) return custom_cmds;

        try {
            boost::json::value jv = boost::json::parse(content);
            const auto& root = jv.as_object();

            for (const auto& [key, val] : root) {
                const auto& obj = val.as_object();
                CustomCommand cmd;

                if (obj.contains("uri")) {
                    cmd.uri = std::string(obj.at("uri").as_string());
                }
                if (obj.contains("local")) {
                    cmd.local = obj.at("local").as_bool();
                }
                if (obj.contains("online")) {
                    cmd.online = obj.at("online").as_bool();
                }
                if (obj.contains("print-message")) {
                    cmd.print_message = std::string(obj.at("print-message").as_string());
                }
                if (obj.contains("Needs-game_name/appid-arg")) {
                    cmd.needs_arg = obj.at("Needs-game_name/appid-arg").as_bool();
                }
                else if (obj.contains("needs-arg")) {
                    cmd.needs_arg = obj.at("needs-arg").as_bool();
                }

                custom_cmds[std::string(key)] = cmd;
            }
        }
        catch (...) {
            // Ignore malformed custom command definitions
        }

        return custom_cmds;
    }

    // Resolves AppID and Game Name, performs placeholders replacement, and executes URI
    inline bool execute_custom_command(const CustomCommand& cmd,
        const std::string& raw_arg,
        std::vector<GameInfo>& installed_games) {
        std::string app_id;
        std::string resolved_name = raw_arg;

        if (cmd.needs_arg) {
            if (raw_arg.empty()) return false;

            // Check direct numerical AppID
            if (!raw_arg.empty() && std::all_of(raw_arg.begin(), raw_arg.end(), ::isdigit)) {
                app_id = raw_arg;
            }
            else {
                // Check local library first if enabled
                if (cmd.local) {
                    GameInfo* game = find_game_by_name(installed_games, raw_arg);

                    // Spell check fallback if not found locally
                    if (!game) {
                        std::vector<std::string> game_names = misc::get_installed_game_names(installed_games);

                        std::string corrected = spell_checker::interactive_closest(raw_arg, game_names);
                        game = find_game_by_name(installed_games, corrected);
                    }

                    if (game) {
                        app_id = game->app_id;
                        resolved_name = game->name;
                    }
                }

                // Fallback / primary online lookup if enabled
                if (app_id.empty() && cmd.online) {
                    app_id = search_appid_online(raw_arg);
                    if (app_id == SEARCH_RESULT_NOT_FOUND) {
                        app_id.clear();
                    }
                }
            }

            if (app_id.empty()) return false;
        }

        std::string final_uri = cmd.uri;
        std::string final_msg = cmd.print_message;

        replace_all(final_uri, "@APP_ID", app_id);
        replace_all(final_uri, "@GAME_NAME", resolved_name);

        replace_all(final_msg, "@APP_ID", app_id);
        replace_all(final_msg, "@GAME_NAME", resolved_name);

        if (!final_msg.empty()) {
            std::cout << final_msg;
        }

        execute_uri(final_uri);
        return true;
    }

}