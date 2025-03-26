#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace utils {
    bool createDirectoryIfNotExists(const std::string& path);
    bool removeDirectoryRecursive(const std::string& path);
    std::vector<std::string> findFiles(const std::string& directory, const std::string& pattern);
    
    std::string replaceAll(std::string str, const std::string& from, const std::string& to);
    bool endsWith(const std::string& str, const std::string& suffix);
    std::string trimEnd(std::string str, char ch);
    
    bool runCommand(const std::string& command, std::string& output);
    bool copyDirectoryRecursive(const std::string& sourceDir, const std::string& destDir, bool overwrite = true);
}
