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

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>

namespace file {

    /**
     * @brief Reads the entire contents of a file into a std::string.
     * @param path The path to the file.
     * @return std::optional containing the file contents, or std::nullopt if reading failed.
     */
    inline std::optional<std::string> get(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            return std::nullopt;
        }

        std::error_code ec;
        auto size = std::filesystem::file_size(path, ec);
        if (ec) {
            return std::nullopt;
        }

        std::string buffer;
        buffer.resize(size);

        if (size > 0 && !file.read(buffer.data(), static_cast<std::streamsize>(size))) {
            return std::nullopt;
        }

        return buffer;
    }

    /**
     * @brief Writes data to a file. Overwrites existing contents by default.
     * @param path The target file path.
     * @param data The content to write.
     * @return true if the write operation succeeded, false otherwise.
     */
    inline bool set(const std::filesystem::path& path, std::string_view data) {
        std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            return false;
        }

        file.write(data.data(), static_cast<std::streamsize>(data.size()));
        return file.good();
    }

}