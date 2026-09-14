#include <iostream>
#include <string>
#include <cstdlib>
#include <string>

#ifdef _WIN32
#include <stdlib.h>
#include <windows.h>
#define ENV_VAR_PATH_SEP ";"
#else
#include <stdlib.h>
#define ENV_VAR_PATH_SEP ":"
#endif

namespace env_vars {
    namespace path {
        bool has(const std::string& THING) {
            const char* env_p = std::getenv("PATH");
            if (!env_p) return false;

            std::string current_path(env_p);
            return current_path.find(THING) != std::string::npos;
        }

        // Appends to the current process environment block
        void append(const std::string& newDir) {
            const char* current_path = std::getenv("PATH");
            std::string updatedPath;

            if (current_path && std::string(current_path).length() > 0) {
                updatedPath = std::string(current_path) + ENV_VAR_PATH_SEP + newDir;
            }
            else {
                updatedPath = newDir;
            }

#ifdef _WIN32
            _putenv_s("PATH", updatedPath.c_str());
#else
            setenv("PATH", updatedPath.c_str(), 1);
#endif
        }

        bool append_permanent_windows(const std::string& newDir) {
#ifdef _WIN32
            HKEY hKey;
            // Open the User environment registry key
            if (RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", 0, KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS) {
                return false;
            }

            char pathBuffer[32767]; // Max environment variable size
            DWORD bufferSize = sizeof(pathBuffer);
            DWORD type = REG_EXPAND_SZ;

            std::string userPath = "";

            // Read the existing User PATH
            if (RegQueryValueExA(hKey, "Path", NULL, &type, (LPBYTE)pathBuffer, &bufferSize) == ERROR_SUCCESS) {
                userPath = pathBuffer;
            }

            // Check if directory already exists in user PATH
            if (userPath.find(newDir) != std::string::npos) {
                RegCloseKey(hKey);
                return true;
            }

            // Append new directory
            if (!userPath.empty() && userPath.back() != ';') {
                userPath += ";";
            }
            userPath += newDir;

            // Write back to Registry
            LONG setRes = RegSetValueExA(hKey, "Path", 0, REG_EXPAND_SZ,
                (const BYTE*)userPath.c_str(),
                static_cast<DWORD>(userPath.length() + 1));

            RegCloseKey(hKey);

            if (setRes == ERROR_SUCCESS) {
                // Broadcast change to active windows/processes
                DWORD_PTR result;
                SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                    (LPARAM)"Environment", SMTO_ABORTIFHUNG, 5000, &result);
                return true;
            }
#endif
            return false;
        }

        // Writes to ~/.bashrc or ~/.zshrc for permanent update across sessions
        bool append_permanent_posix(const std::string& newDir) {
#ifndef _WIN32
            const char* homeDir = std::getenv("HOME");
            if (!homeDir) return false;

            std::vector<std::string> profileFiles = {
                std::string(homeDir) + "/.zshrc",
                std::string(homeDir) + "/.bashrc"
            };

            std::string exportLine = "\nexport PATH=\"$PATH:" + newDir + "\"\n";
            bool updatedAny = false;

            for (const auto& filePath : profileFiles) {
                if (std::filesystem::exists(filePath)) {
                    std::ofstream profileFile(filePath, std::ios::app);
                    if (profileFile.is_open()) {
                        profileFile << exportLine;
                        profileFile.close();
                        updatedAny = true;
                    }
                }
            }

            if (updatedAny) {
                const char* current_path = std::getenv("PATH");
                std::string updatedPath = std::string(current_path ? current_path : "") + ":" + newDir;
                setenv("PATH", updatedPath.c_str(), 1);
            }

            return updatedAny;
#endif
            return false;
        }

        bool append_permanent(const std::string& newDir) {
#ifdef _WIN32
            return append_permanent_windows(newDir);
#else
            return append_permanent_posix(newDir);
#endif
        }
    }
}