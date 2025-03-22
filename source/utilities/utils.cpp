#include "utils.hpp"
#include <algorithm>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace utils {
    bool createDirectoryIfNotExists(const std::string& path) {
        try {
            return std::filesystem::create_directories(path);
        } catch (...) {
            return false;
        }
    }

    bool removeDirectoryRecursive(const std::string& path) {
        try {
            return std::filesystem::remove_all(path) > 0;
        } catch (...) {
            return false;
        }
    }

    std::vector<std::string> findFiles(const std::string& directory, const std::string& pattern) {
        std::vector<std::string> files;
        try {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (pattern.empty() || filename.find(pattern) != std::string::npos) {
                        files.push_back(entry.path().string());
                    }
                }
            }
        } catch (...) {}
        return files;
    }

    std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }

    bool endsWith(const std::string& str, const std::string& suffix) {
        if (str.length() < suffix.length()) {
            return false;
        }
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }

    std::string trimEnd(std::string str, char ch) {
        str.erase(std::find_if(str.rbegin(), str.rend(), [ch](char c) {
            return c != ch;
        }).base(), str.end());
        return str;
    }

    bool runCommand(const std::string& command, std::string& output) {
        output.clear();
        
#ifdef _WIN32
        HANDLE hReadPipe, hWritePipe;
        SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
        
        if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
            return false;
        }

        STARTUPINFOA si = { sizeof(STARTUPINFOA) };
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        si.wShowWindow = SW_HIDE;
        si.hStdOutput = hWritePipe;
        si.hStdError = hWritePipe;

        PROCESS_INFORMATION pi = { 0 };
        std::string cmdLine = "cmd.exe /c " + command;
        
        if (!CreateProcessA(nullptr, const_cast<LPSTR>(cmdLine.c_str()), 
            nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
            CloseHandle(hReadPipe);
            CloseHandle(hWritePipe);
            return false;
        }

        CloseHandle(hWritePipe);
        
        char buffer[4096];
        DWORD bytesRead;
        
        while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr)) {
            if (bytesRead == 0) break;
            buffer[bytesRead] = 0;
            output += buffer;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hReadPipe);
        
        return exitCode == 0;
#else
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            return false;
        }

        char buffer[4096];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            output += buffer;
        }

        return pclose(pipe) == 0;
#endif
    }
}
