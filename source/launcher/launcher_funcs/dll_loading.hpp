#pragma once
#include <string>
#include <filesystem>

namespace DllLoading {
    enum class Result {
        Success,
        FileNotFound,
        InvalidGamePath,
        ZipError
    };

    bool isGameRunning();

    Result extractDlls(const std::string& gameDir, bool isOnline, bool reshadeEnabled);

    bool launchGame(const std::string& gameExePath, bool isOnline);
}