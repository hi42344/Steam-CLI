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

namespace misc {
    namespace string {
        inline void replace_all(std::string& str, const std::string& from, const std::string& to) {
            size_t start_pos = 0;
            while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length();
            }
        }

        inline bool starts_with(const std::string& str, const std::string& thing) {
            if (str.length() < thing.length()) {
                return false;
            }

            bool starts_with_thing = true;
            for (size_t i = 0; i < thing.length(); i++) {
                if (!(std::to_string(str[i]) == std::to_string(thing[i]))) {
                    starts_with_thing = false;
                }
            }
            return starts_with_thing;
        }

        //Normalizes path and uppercases 'c:' if it is the starting part
        inline std::string path_clean_up(std::string str) {
            if (starts_with(str, "c:")) {
                str.replace(0, 1, "C");
            }
            replace_all(str, "\\", "/");
            return str;
        }
    }
}