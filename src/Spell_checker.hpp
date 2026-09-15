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
#include "helpers/Easy_to_use_levenshtien.hpp"
#include "helpers/Misc.hpp"
#include "helpers/Colors.hpp"
#include <cmath>
#include <algorithm>

namespace spell_checker {
    inline bool is_close(const std::string& s1, const std::string& s2, double max_ratio = 0.35) {
        const std::size_t len1 = s1.length();
        const std::size_t len2 = s2.length();
        const std::size_t max_len = std::max(len1, len2);

        if (max_len == 0) return true;

        const std::size_t allowed_distance = static_cast<std::size_t>(max_len * max_ratio);
        const std::size_t length_diff = (len1 > len2) ? (len1 - len2) : (len2 - len1);

        if (length_diff > allowed_distance) return false;

        return libraries::levenshteins_distance(s1, s2) <= allowed_distance;
    }

    inline std::vector<std::string> get_closest(const std::string& from, const std::vector<std::string>& to_cmp, const double max_close_ratio = 0.40, const size_t max_amount_of_closests = -1) {
        std::vector<std::string> return_arr;
        std::vector<std::string> already_found;

        if (!to_cmp.empty()) {
            for (const std::string val : to_cmp) {
                bool _is_close = is_close(from, val, max_close_ratio);

                if (max_amount_of_closests == -1 || return_arr.size() < max_amount_of_closests) {
                    if (_is_close) {
                        if (std::find(already_found.begin(), already_found.end(), val) == already_found.end()) {
                            return_arr.push_back(val);
                            already_found.push_back(val);
                        }
                    }
                }
                else {
                    break;
                }
            }
        }

        return return_arr;
    }

    /// @return returns either the closest/one of the closest or from
    inline std::string interactive_closest(const std::string& from, const std::vector<std::string>& list, const int max_bad_inputs = 2, const double max_ratio = 0.45) {
        std::vector<std::string> closest = get_closest(from, list, max_ratio);
        if (closest.empty()) {
            return from;
        }

        int AMOUNT_OF_BAD_INPUTS = 0;

        std::cout << color::muted << "(Y/n/x)" << color::reset << "\n";

        for (size_t i = 0; i < closest.size(); i++) {
            if (AMOUNT_OF_BAD_INPUTS >= max_bad_inputs) {
                std::cout << color::bold << color::error << "Exiting" << color::reset << '\n';
                return from;
            }

            // Highlight game title
            std::string styled_title = std::string(color::bold) + color::steam_blue + "\"" + closest[i] + "\"" + color::reset;

            if (i == 0) {
                std::cout << "Did you mean " << color::reset
                    << styled_title << "?\n" << color::reset
                    << color::bold << "> " << color::reset;
            }
            else {
                std::cout << styled_title << "?\n" << color::reset
                    << color::bold << "> " << color::reset;
            }

            std::string line;
            if (!std::getline(std::cin, line)) {
                return from;
            }

            if (line.empty()) {
                return closest[i];
            }

            int to_lower_char = std::tolower(static_cast<unsigned char>(line[0]));
            if (to_lower_char == 'y') {
                return closest[i];
            }
            else if (to_lower_char == 'x') {
                return from;
            }
            else if (to_lower_char != 'n') {
                AMOUNT_OF_BAD_INPUTS++;
                if (AMOUNT_OF_BAD_INPUTS < max_bad_inputs) {
                    //Give a warning if giving a bad input
                    std::cout << color::bold << color::warning << "Warning: do not enter more than " << max_bad_inputs << " bad inputs" << color::reset << '\n';
                }
                i--;
            }
        }

        return from;
    }
}