#include "std_include.hpp"

#include "auto_update.hpp"
#include <wininet.h>
#include <fstream>
#include <sstream>
#include <filesystem>

#pragma comment(lib, "wininet.lib")

//todo : test

namespace fs = std::filesystem;

namespace AutoUpdate {
    constexpr const wchar_t* GITHUB_API = L"api.github.com";
    constexpr const wchar_t* REPO_PATH = L"/repos/bodnjenie14/Project_-bo4_Launcher/releases/latest";

    std::wstring getLatestVersion() {
        HINTERNET internet = InternetOpenW(L"Shield Launcher", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
        if (!internet) return L"";

        HINTERNET connect = InternetConnectW(internet, GITHUB_API, INTERNET_DEFAULT_HTTPS_PORT,
                                           nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
        if (!connect) {
            InternetCloseHandle(internet);
            return L"";
        }

        HINTERNET request = HttpOpenRequestW(connect, L"GET", REPO_PATH, nullptr,
                                           nullptr, nullptr, INTERNET_FLAG_SECURE, 0);
        if (!request) {
            InternetCloseHandle(connect);
            InternetCloseHandle(internet);
            return L"";
        }

        const wchar_t* headers = L"Accept: application/vnd.github.v3+json\r\n"
                                L"User-Agent: Shield-Launcher\r\n";
        HttpAddRequestHeadersW(request, headers, -1, HTTP_ADDREQ_FLAG_ADD);

        if (!HttpSendRequestW(request, nullptr, 0, nullptr, 0)) {
            InternetCloseHandle(request);
            InternetCloseHandle(connect);
            InternetCloseHandle(internet);
            return L"";
        }

        std::string response;
        char buffer[4096];
        DWORD bytesRead;

        while (InternetReadFile(request, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
            response.append(buffer, bytesRead);
        }

        InternetCloseHandle(request);
        InternetCloseHandle(connect);
        InternetCloseHandle(internet);


        size_t pos = response.find("\"tag_name\":\"");
        if (pos != std::string::npos) {
            pos += 11;
            size_t endPos = response.find("\"", pos);
            if (endPos != std::string::npos) {
                std::string version = response.substr(pos, endPos - pos);
                return std::wstring(version.begin(), version.end());
            }
        }

        return L"";
    }

    bool checkForUpdate(const std::wstring& currentVersion) {
        std::wstring latestVersion = getLatestVersion();
        if (latestVersion.empty()) return false;
        
        return latestVersion != currentVersion;
    }

    bool downloadUpdate(const std::wstring& downloadUrl, 
                       const std::function<void(const UpdateProgress&)>& progressCallback) {
        HINTERNET internet = InternetOpenW(L"Shield Launcher", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
        if (!internet) return false;

        HINTERNET file = InternetOpenUrlW(internet, downloadUrl.c_str(), nullptr, 0,
                                        INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
        if (!file) {
            InternetCloseHandle(internet);
            return false;
        }

        DWORD fileSize = 0;
        DWORD sizeSize = sizeof(fileSize);
        HttpQueryInfoW(file, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
                      &fileSize, &sizeSize, nullptr);

        wchar_t tempPath[MAX_PATH];
        GetTempPathW(MAX_PATH, tempPath);
        wchar_t tempFileName[MAX_PATH];
        GetTempFileNameW(tempPath, L"SHL", 0, tempFileName);

        std::ofstream outFile(tempFileName, std::ios::binary);
        if (!outFile) {
            InternetCloseHandle(file);
            InternetCloseHandle(internet);
            return false;
        }

        UpdateProgress progress = {true, 0, fileSize};
        char buffer[8192];
        DWORD bytesRead;

        while (InternetReadFile(file, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
            outFile.write(buffer, bytesRead);
            progress.bytesDownloaded += bytesRead;
            progressCallback(progress);
        }

        outFile.close();
        InternetCloseHandle(file);
        InternetCloseHandle(internet);

        progress.inProgress = false;
        progressCallback(progress);

        return true;
    }

    bool createUpdateScript(const std::wstring& currentFolder,
                          const std::wstring& updaterFolder,
                          const std::wstring& programName) {
        std::wstring scriptPath = updaterFolder + L"\\update.bat";
        std::wofstream script(scriptPath);
        if (!script) return false;

        script << L"@echo off\n"
               << L"timeout /t 2 /nobreak > nul\n"
               << L"xcopy /y /e \"" << updaterFolder << L"\\*\" \"" << currentFolder << L"\" > nul\n"
               << L"start \"\" \"" << currentFolder << L"\\" << programName << L"\"\n"
               << L"rmdir /s /q \"" << updaterFolder << L"\"\n"
               << L"del \"%~f0\"\n";

        return true;
    }
}