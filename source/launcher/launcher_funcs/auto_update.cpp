#include <std_include.hpp>
#include "auto_update.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <wininet.h>
#include <shlobj.h>
#pragma comment(lib, "wininet.lib")
#include "configuration.hpp"
#include "debugging/serverlog.hpp"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QWidget>
#include <filesystem>

namespace updater {

    const std::string SERVER_VERSION = "1.0.19.3";

	//test repo for testing update soon as i fluffed it
    //const std::string GITHUB_API_URL = "https://api.github.com/repos/bodnjenie14/bo4-test/releases/latest";
    //const std::string DOWNLOAD_URL_BASE = "https://github.com/bodnjenie14/bo4-test/releases/download/";


    const std::string GITHUB_API_URL = "https://api.github.com/repos/bodnjenie14/Project_-bo4_Launcher/releases/latest";
    const std::string DOWNLOAD_URL_BASE = "https://github.com/bodnjenie14/Project_-bo4_Launcher/releases/download/";


    bool check_for_updates() {
        logger::write(logger::LOG_LEVEL_DEBUG, logger::LOG_LABEL_INITIALIZER, "Checking GitHub API: %s", GITHUB_API_URL.c_str());

        // Check if we've already prompted for this update
        std::string lastPromptedVersion = utils::configuration::ReadString("UpdateInfo", "LastPromptedVersion", "");

        HINTERNET hInternet = InternetOpenA("TSTO-Server-Updater", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) {
            logger::write(logger::LOG_LEVEL_ERROR, logger::LOG_LABEL_INITIALIZER, "Failed to initialize WinINet");
            return false;
        }

        HINTERNET hConnect = InternetOpenUrlA(hInternet, GITHUB_API_URL.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (!hConnect) {
            logger::write(logger::LOG_LEVEL_ERROR, logger::LOG_LABEL_INITIALIZER, "Failed to connect to GitHub API");
            InternetCloseHandle(hInternet);
            return false;
        }

        char buffer[4096];
        DWORD bytesRead;
        std::string response;

        while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
            response.append(buffer, bytesRead);
        }

        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        size_t tagPos = response.find("\"tag_name\":");
        size_t assetPos = response.find("\"name\":", response.find("\"assets\":"));

        if (tagPos != std::string::npos && assetPos != std::string::npos) {
            size_t startPos = response.find("\"", tagPos + 11) + 1;
            size_t endPos = response.find("\"", startPos);
            std::string latestVersion = response.substr(startPos, endPos - startPos);

            startPos = response.find("\"", assetPos + 7) + 1;
            endPos = response.find("\"", startPos);
            std::string assetName = response.substr(startPos, endPos - startPos);

            logger::write(logger::LOG_LEVEL_INFO, logger::LOG_LABEL_INITIALIZER,
                "Current version: %s, Latest version: %s, Asset: %s",
                SERVER_VERSION.c_str(), latestVersion.c_str(), assetName.c_str());

            utils::configuration::WriteString("UpdateInfo", "LatestVersion", latestVersion);
            utils::configuration::WriteString("UpdateInfo", "AssetName", assetName);

            if (lastPromptedVersion == latestVersion) {
                logger::write(logger::LOG_LEVEL_INFO, logger::LOG_LABEL_INITIALIZER,
                    "Already prompted for version %s, skipping update check", latestVersion.c_str());
                return false;
            }

            // Extract version from asset name for special release types
            if (latestVersion == "alpha" || latestVersion == "beta" || latestVersion == "release" || latestVersion == "Release") {
                logger::write(logger::LOG_LEVEL_DEBUG, logger::LOG_LABEL_INITIALIZER,
                    "Special release type: %s. Extracting version from asset name: %s",
                    latestVersion.c_str(), assetName.c_str());

                std::string versionStr;
                size_t vPos = assetName.find("Update_");
                if (vPos != std::string::npos) {
                    size_t startPos = vPos + 7; // Skip "Update_"
                    size_t endPos = assetName.find(".zip", startPos);
                    if (endPos != std::string::npos) {
                        versionStr = assetName.substr(startPos, endPos - startPos);
                        logger::write(logger::LOG_LEVEL_DEBUG, logger::LOG_LABEL_INITIALIZER,
                            "Extracted version string from asset name: %s", versionStr.c_str());
                    }
                }

                if (versionStr.empty()) {
                    logger::write(logger::LOG_LEVEL_ERROR, logger::LOG_LABEL_INITIALIZER,
                        "Failed to extract version from asset name");
                    return false;
                }

                // If the extracted version matches SERVER_VERSION, no update needed
                if (versionStr == SERVER_VERSION) {
                    logger::write(logger::LOG_LEVEL_INFO, logger::LOG_LABEL_INITIALIZER,
                        "No update needed - current version is up to date");
                    logger::write(logger::LOG_LEVEL_INFO, logger::LOG_LABEL_INITIALIZER,
                        "Running latest version");
                    return false;
                }
            }

            std::string currentVer = SERVER_VERSION;
            std::string latestVer = latestVersion;
            if (currentVer[0] == 'v') currentVer = currentVer.substr(1);
            if (latestVer[0] == 'v') latestVer = latestVer.substr(1);

            bool isNewer = false;
            try {
                float currentNum = std::stof(currentVer);
                float latestNum = std::stof(latestVer);
                isNewer = latestNum > currentNum;
            }
            catch (const std::exception& e) {
                logger::write(logger::LOG_LEVEL_WARN, logger::LOG_LABEL_INITIALIZER,
                    "Float conversion failed for version comparison: %s. Falling back to string comparison.", e.what());

                std::vector<std::string> currentParts;
                std::vector<std::string> latestParts;

                std::string currentVerCopy = currentVer;
                size_t pos = 0;
                while ((pos = currentVerCopy.find('.')) != std::string::npos) {
                    currentParts.push_back(currentVerCopy.substr(0, pos));
                    currentVerCopy.erase(0, pos + 1);
                }
                if (!currentVerCopy.empty()) {
                    currentParts.push_back(currentVerCopy);
                }

                std::string latestVerCopy = latestVer;
                pos = 0;
                while ((pos = latestVerCopy.find('.')) != std::string::npos) {
                    latestParts.push_back(latestVerCopy.substr(0, pos));
                    latestVerCopy.erase(0, pos + 1);
                }
                if (!latestVerCopy.empty()) {
                    latestParts.push_back(latestVerCopy);
                }

                size_t minSize = std::min(currentParts.size(), latestParts.size());
                for (size_t i = 0; i < minSize; i++) {
                    try {
                        int currentNum = std::stoi(currentParts[i]);
                        int latestNum = std::stoi(latestParts[i]);

                        if (latestNum > currentNum) {
                            isNewer = true;
                            break;
                        }
                        else if (latestNum < currentNum) {
                            isNewer = false;
                            break;
                        }
                    }
                    catch (const std::exception& e) {
                        if (latestParts[i] > currentParts[i]) {
                            isNewer = true;
                            break;
                        }
                        else if (latestParts[i] < currentParts[i]) {
                            isNewer = false;
                            break;
                        }
                    }
                }

                if (!isNewer && currentParts.size() < latestParts.size()) {
                    isNewer = true;
                }
            }

            return isNewer;
        }

        logger::write(logger::LOG_LEVEL_ERROR, logger::LOG_LABEL_INITIALIZER, "Failed to parse GitHub API response");
        return false;
    }

    std::string get_server_version() {
        return SERVER_VERSION;
    }

    bool check_and_prompt_for_updates(QWidget* parent) {
        if (check_for_updates()) {
            std::string latestVersion = utils::configuration::ReadString("UpdateInfo", "LatestVersion", "release");
            std::string assetName = utils::configuration::ReadString("UpdateInfo", "AssetName", "");

            utils::configuration::WriteString("UpdateInfo", "LastPromptedVersion", assetName);

            QMessageBox msgBox(parent);
            msgBox.setWindowTitle("Update Available");
            msgBox.setText(QString("A new version (%1) is available. Your current version is %2.")
                .arg(QString::fromStdString(latestVersion))
                .arg(QString::fromStdString(SERVER_VERSION)));
            msgBox.setInformativeText("Do you want to update now?");
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::Yes);

            int ret = msgBox.exec();

            if (ret == QMessageBox::Yes) {
                logger::write(logger::LOG_LEVEL_INFO, logger::LOG_LABEL_INITIALIZER, "User chose to update now");
                download_and_update();
                return true;
            }
            else {
                logger::write(logger::LOG_LEVEL_INFO, logger::LOG_LABEL_INITIALIZER, "User chose to skip update");
                return false;
            }
        }

        logger::write(logger::LOG_LEVEL_INFO, logger::LOG_LABEL_INITIALIZER, "No updates available");
        return false;
    }

    void download_and_update() {
        std::string latestVersion = utils::configuration::ReadString("UpdateInfo", "LatestVersion", "release");
        std::string assetName = utils::configuration::ReadString("UpdateInfo", "AssetName", "Project_BO4_Launcher_Update_1.0.18.1.zip");
        logger::write(logger::LOG_LEVEL_INFO, logger::LOG_LABEL_INITIALIZER, "Starting update process to version %s", latestVersion.c_str());
        logger::write(logger::LOG_LEVEL_DEBUG, logger::LOG_LABEL_INITIALIZER, "Asset name: %s", assetName.c_str());

        char tempPath[MAX_PATH];
        GetTempPathA(MAX_PATH, tempPath);
        std::string updateDir = std::string(tempPath) + "\\bo4_update";
        CreateDirectoryA(updateDir.c_str(), NULL);

        std::string downloadUrl = DOWNLOAD_URL_BASE + latestVersion + "/" + assetName;
        std::string zipPath = updateDir + "\\update.zip";

        logger::write(logger::LOG_LEVEL_DEBUG, logger::LOG_LABEL_INITIALIZER, "Download URL: %s", downloadUrl.c_str());
        logger::write(logger::LOG_LEVEL_DEBUG, logger::LOG_LABEL_INITIALIZER, "Update directory: %s", updateDir.c_str());
        logger::write(logger::LOG_LEVEL_DEBUG, logger::LOG_LABEL_INITIALIZER, "Zip path: %s", zipPath.c_str());

        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        std::string currentExe = std::string(exePath);
        std::string exeDir = currentExe.substr(0, currentExe.find_last_of("\\"));
        std::string exeName = currentExe.substr(currentExe.find_last_of("\\") + 1);
        std::string projectDir = exeDir.substr(0, exeDir.find_last_of("\\"));  // Get parent directory (project-bo4)
        std::string updateBatchPath = updateDir + "\\update.bat";

        logger::write(logger::LOG_LEVEL_DEBUG, logger::LOG_LABEL_INITIALIZER, "Current exe: %s", currentExe.c_str());
        logger::write(logger::LOG_LEVEL_DEBUG, logger::LOG_LABEL_INITIALIZER, "Exe directory: %s", exeDir.c_str());
        logger::write(logger::LOG_LEVEL_DEBUG, logger::LOG_LABEL_INITIALIZER, "Project directory: %s", projectDir.c_str());
        logger::write(logger::LOG_LEVEL_DEBUG, logger::LOG_LABEL_INITIALIZER, "Exe name: %s", exeName.c_str());

        logger::write(logger::LOG_LEVEL_INFO, logger::LOG_LABEL_UPDATE, "[UPDATE] Downloading update file...");

        HINTERNET hInternet = InternetOpenA("BO4-Launcher-Updater", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) {
            logger::write(logger::LOG_LEVEL_ERROR, logger::LOG_LABEL_UPDATE, "[ERROR] Failed to initialize WinINet for download");
            return;
        }

        HINTERNET hConnect = InternetOpenUrlA(hInternet, downloadUrl.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
        if (!hConnect) {
            logger::write(logger::LOG_LEVEL_ERROR, logger::LOG_LABEL_UPDATE, "[ERROR] Failed to connect to download URL");
            InternetCloseHandle(hInternet);
            return;
        }

        HANDLE hFile = CreateFileA(zipPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            logger::write(logger::LOG_LEVEL_ERROR, logger::LOG_LABEL_UPDATE, "[ERROR] Failed to create update zip file");
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return;
        }

        char buffer[8192];
        DWORD bytesRead = 0;
        DWORD bytesWritten = 0;
        BOOL readResult = FALSE;

        do {
            readResult = InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead);
            if (readResult && bytesRead > 0) {
                WriteFile(hFile, buffer, bytesRead, &bytesWritten, NULL);
            }
        } while (readResult && bytesRead > 0);

        CloseHandle(hFile);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        if (!std::filesystem::exists(zipPath)) {
            logger::write(logger::LOG_LEVEL_ERROR, logger::LOG_LABEL_UPDATE, "[ERROR] Download failed - zip file not found");
            return;
        }

        logger::write(logger::LOG_LEVEL_INFO, logger::LOG_LABEL_UPDATE, "[UPDATE] Download complete. Extracting files...");


        std::string batchContent = "@echo off\r\n";
        batchContent += "echo Waiting for launcher to close...\r\n";
        batchContent += "ping -n 5 127.0.0.1 > nul\r\n"; 

        batchContent += "echo Extracting update files...\r\n";
        batchContent += "powershell -Command \"Expand-Archive -Path '" + zipPath + "' -DestinationPath '" + updateDir + "' -Force\"\r\n";
        batchContent += "if %ERRORLEVEL% NEQ 0 (\r\n";
        batchContent += "  echo Failed to extract update files!\r\n";
        batchContent += "  pause\r\n";
        batchContent += "  exit /b 1\r\n";
        batchContent += ")\r\n";

        batchContent += "echo Updating files...\r\n";
        batchContent += "taskkill /f /im \"" + exeName + "\" > nul 2>&1\r\n";
        batchContent += "ping -n 2 127.0.0.1 > nul\r\n";
        
        batchContent += "echo Checking for update files...\r\n";
        batchContent += "if exist \"" + updateDir + "\\Project_BO4\\*\" (\r\n";
        batchContent += "  echo Found nested Project_BO4 folder\r\n";
        batchContent += "  xcopy /s /y \"" + updateDir + "\\Project_BO4\\*\" \"" + projectDir + "\\\" > nul\r\n";
        batchContent += ") else if exist \"" + updateDir + "\\project-bo4\\*\" (\r\n";
        batchContent += "  echo Found nested project-bo4 folder\r\n";
        batchContent += "  xcopy /s /y \"" + updateDir + "\\project-bo4\\*\" \"" + projectDir + "\\\" > nul\r\n";
        batchContent += ") else if exist \"" + updateDir + "\\launcher\\*\" (\r\n";
        batchContent += "  echo Found launcher folder\r\n";
        batchContent += "  xcopy /s /y \"" + updateDir + "\\launcher\\*\" \"" + exeDir + "\\\" > nul\r\n";
        batchContent += "  for /d %%i in (\"" + updateDir + "\\*\") do (\r\n";
        batchContent += "    if not \"%%~nxi\"==\"launcher\" xcopy /s /y \"%%i\\*\" \"" + projectDir + "\\%%~nxi\\\" > nul\r\n";
        batchContent += "  )\r\n";
        batchContent += ") else (\r\n";
        batchContent += "  echo Using root folder\r\n";
        batchContent += "  xcopy /s /y \"" + updateDir + "\\*\" \"" + projectDir + "\\\" > nul\r\n";
        batchContent += ")\r\n";
        
        batchContent += "if %ERRORLEVEL% NEQ 0 (\r\n";
        batchContent += "  echo Failed to copy update files!\r\n";
        batchContent += "  pause\r\n";
        batchContent += "  exit /b 1\r\n";
        batchContent += ")\r\n";

        batchContent += "echo Cleaning up...\r\n";
        batchContent += "del \"" + zipPath + "\" > nul\r\n";
        batchContent += "rmdir /s /q \"" + updateDir + "\" > nul\r\n";

        batchContent += "echo Starting launcher...\r\n";
        batchContent += "start \"\" \"" + exeDir + "\\" + exeName + "\"\r\n";
        batchContent += "echo Update complete!\r\n";
        batchContent += "timeout /t 3\r\n";
        batchContent += "del \"%~f0\"\r\n";
        batchContent += "exit\r\n";

        std::ofstream batchFile(updateBatchPath);
        batchFile << batchContent;
        batchFile.close();

        logger::write(logger::LOG_LEVEL_INFO, logger::LOG_LABEL_UPDATE, "[UPDATE] Created update script at %s", updateBatchPath.c_str());
        logger::write(logger::LOG_LEVEL_INFO, logger::LOG_LABEL_UPDATE, "[UPDATE] Launching update process and closing launcher...");

        SHELLEXECUTEINFOA sei = { 0 };
        sei.cbSize = sizeof(SHELLEXECUTEINFOA);
        sei.fMask = SEE_MASK_NOCLOSEPROCESS;
        sei.hwnd = NULL;
        sei.lpVerb = "open";
        sei.lpFile = updateBatchPath.c_str();
        sei.lpParameters = "";
        sei.lpDirectory = NULL;
        sei.nShow = SW_SHOW;  

        if (ShellExecuteExA(&sei)) {
            logger::write(logger::LOG_LEVEL_INFO, logger::LOG_LABEL_UPDATE, "[UPDATE] Update script started successfully");

            Sleep(1000);

            exit(0);
        }
        else {
            logger::write(logger::LOG_LEVEL_ERROR, logger::LOG_LABEL_UPDATE,
                "[ERROR] Failed to start update script. Error code: %d", GetLastError());
        }
    }
}