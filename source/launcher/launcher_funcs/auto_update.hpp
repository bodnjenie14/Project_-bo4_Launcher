#pragma once
#include "../std_include.hpp"
#include <functional>

namespace AutoUpdate {
    struct UpdateProgress {
        bool inProgress;
        size_t bytesDownloaded;
        size_t totalBytes;
    };

    // Check if an update is available
    bool checkForUpdate(const std::wstring& currentVersion);

    // Download and install update
    bool downloadUpdate(const std::wstring& downloadUrl, 
                       const std::function<void(const UpdateProgress&)>& progressCallback);

    // Get the latest release version from GitHub
    std::wstring getLatestVersion();

    // Create update script
    bool createUpdateScript(const std::wstring& currentFolder,
                          const std::wstring& updaterFolder,
                          const std::wstring& programName);
}