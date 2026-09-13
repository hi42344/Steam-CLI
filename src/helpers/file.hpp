#pragma once
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <stdexcept>

inline void save_file(std::filesystem::path path, const std::string& contents) {
    // Ensure parent directories exist before creating the file
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + path.string());
    }

    file << contents;
}

inline std::string load_file(std::filesystem::path path) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("File does not exist: " + path.string());
    }

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + path.string());
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}