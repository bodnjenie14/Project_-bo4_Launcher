#include "std_include.hpp"
#include "dll_loading.hpp"
#include <Windows.h>
#include <TlHelp32.h>
#include <filesystem>
#include <fstream>
#include <zlib.h>
#include <QCoreApplication>
#include <thread>
#include <atomic>

namespace fs = std::filesystem;

namespace DllLoading {
    std::thread cleanupThread;
    std::atomic<bool> cleanupThreadRunning(false);
    std::string gameDirectoryForCleanup;  
    DWORD gameProcessId = 0;  

    void monitorGameAndCleanup(const std::string& gamePath, DWORD processId) {
        cleanupThreadRunning = true;
        std::cout << "Starting cleanup monitor thread for process ID: " << processId << std::endl;
        
        gameProcessId = processId;
        
        HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, processId);
        if (hProcess == NULL) {
            std::cout << "Failed to open process handle: " << GetLastError() << std::endl;
            while (isGameRunning()) {
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        } else {

            std::cout << "Waiting for game process to exit..." << std::endl;
            WaitForSingleObject(hProcess, INFINITE);
            CloseHandle(hProcess);
        }
        
        gameProcessId = 0;  
        std::cout << "Game has terminated. Cleaning up DLLs..." << std::endl;
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        fs::path gameDir = fs::path(gamePath);
        fs::path dllPath = gameDir / "XInput9_1_0.dll";
        
        try {
            if (fs::exists(dllPath)) {
                fs::remove(dllPath);
                std::cout << "Successfully deleted: " << dllPath.string() << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Error deleting DLLs: " << e.what() << std::endl;
        }
        
        cleanupThreadRunning = false;
    }

    bool isGameRunning() {
        if (gameProcessId != 0) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, gameProcessId);
            if (hProcess != NULL) {
                DWORD exitCode;
                if (GetExitCodeProcess(hProcess, &exitCode) && exitCode == STILL_ACTIVE) {
                    CloseHandle(hProcess);
                    return true;
                }
                CloseHandle(hProcess);
                return false;
            }
        }
        
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) return false;

        PROCESSENTRY32W processEntry;
        processEntry.dwSize = sizeof(processEntry);

        if (!Process32FirstW(snapshot, &processEntry)) {
            CloseHandle(snapshot);
            return false;
        }

        bool found = false;
        do {
            if (_wcsicmp(processEntry.szExeFile, L"BlackOps4.exe") == 0) {
                found = true;
                break;
            }
        } while (Process32NextW(snapshot, &processEntry));

        CloseHandle(snapshot);
        return found;
    }

    Result extractDlls(const std::string& gameDir, bool isOnline, bool reshadeEnabled) {
        if (gameDir.empty()) {
            return Result::InvalidGamePath;
        }

        fs::path gamePath = fs::path(gameDir);
        fs::path gameExePath = gamePath / "BlackOps4.exe";
        
        std::cout << "Initial game path: " << gamePath.string() << std::endl;
        std::cout << "Looking for game exe at: " << gameExePath.string() << std::endl;
        
        if (!fs::exists(gameExePath)) {
            fs::path parentPath = gamePath.parent_path().parent_path();
            gameExePath = parentPath / "BlackOps4.exe";
            
            std::cout << "Trying parent path: " << parentPath.string() << std::endl;
            std::cout << "Looking for game exe at: " << gameExePath.string() << std::endl;
            
            if (!fs::exists(gameExePath)) {
                std::cout << "Game exe not found at parent path either" << std::endl;
                return Result::InvalidGamePath;
            }
            
            gamePath = parentPath;
            std::cout << "Updated game path to: " << gamePath.string() << std::endl;
        }

        fs::path launcherDir = gamePath / "project-bo4" / "launcher";
        std::cout << "Launcher directory: " << launcherDir.string() << std::endl;
        
        fs::path zipPath = launcherDir / (isOnline ? "mp.zip" : "solo.zip");
        std::cout << "Looking for zip at: " << zipPath.string() << std::endl;
        std::cout << "File exists: " << (fs::exists(zipPath) ? "Yes" : "No") << std::endl;
        
        if (!fs::exists(zipPath)) {
            return Result::FileNotFound;
        }


        // Note: not needed anymore cause of ATE47's plugin now loading zone from project-bo4/zone

        /*
        // zone shit
        std::cout << "Copying zone folders..." << std::endl;

        // --- Copy project-bo4/zone to <gameDir>/zone ---
        try {
            fs::path sourceZone = gamePath / "project-bo4" / "zone";
            fs::path targetZone = gamePath / "zone";

            if(fs::exists(sourceZone)) {
                std::cout << "Copying zone files from: " << sourceZone.string() << " to: " << targetZone.string() << std::endl;
                fs::create_directories(targetZone); // target exists?

                for(const auto& entry : fs::directory_iterator(sourceZone)) {
                    fs::path dest = targetZone / entry.path().filename();
                    std::cout << "Copying: " << entry.path().string() << " -> " << dest.string() << std::endl;
                    fs::copy(entry.path(), dest, fs::copy_options::overwrite_existing);
                }
            }
            else {
                std::cout << "Source zone directory does not exist: " << sourceZone.string() << std::endl;
            }
        }
        catch(const std::exception& e) {
            std::cout << "Failed to copy zone files: " << e.what() << std::endl;
        }
        */

        // delete the old support file, to avoid some getting ui errors cause of two zone files
        fs::path supportFF = gamePath / "zone" / "support.ff";
        try {
            if(fs::exists(supportFF)) {
                std::cout << "Deleting: " << supportFF.string() << std::endl;
                fs::remove(supportFF);
            }
            else
                std::cout << "Failed to delete support.ff, it does not exists.." << std::endl;
        }
        catch(const std::exception& e) {
            std::cout << "Failed to delete support.ff: " << e.what() << std::endl;
        }

        fs::path dllPath = gamePath / "XInput9_1_0.dll";
        if (fs::exists(dllPath)) {
            std::cout << "DLL already exists, will still extraction" << std::endl;
            //return Result::Success;
        }

        try {
            std::string extractCmd = "powershell -Command \"";
            extractCmd += "Expand-Archive -Path '" + zipPath.string() + "' -DestinationPath '" + gamePath.string() + "' -Force";
            extractCmd += "\"";
            
            std::cout << "Executing extraction command: " << extractCmd << std::endl;
            
            int result = system(extractCmd.c_str());
            if (result != 0) {
                std::cout << "Extraction failed with code: " << result << std::endl;
                return Result::ZipError;
            }
            
            fs::path extractedDll = gamePath / "XInput9_1_0.dll";
            if (!fs::exists(extractedDll)) {
                std::cout << "DLL was not extracted successfully" << std::endl;
                return Result::FileNotFound;
            }
            
            std::cout << "Extraction successful" << std::endl;
            return Result::Success;
        }
        catch (const std::exception& e) {
            std::cout << "Exception: " << e.what() << std::endl;
            return Result::ZipError;
        }
        catch (...) {
            std::cout << "Unknown exception" << std::endl;
            return Result::ZipError;
        }
    }

    void cleanupDllsAfterGame() {
        cleanupThreadRunning = true;
        std::cout << "Starting DLL cleanup monitor thread..." << std::endl;
        std::cout << "Will clean up DLLs in directory: " << gameDirectoryForCleanup << std::endl;
        
        while (isGameRunning()) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        
        std::cout << "Game has terminated. Cleaning up DLLs..." << std::endl;
        
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        if (gameDirectoryForCleanup.empty()) {
            std::cout << "Error: Game directory path is empty" << std::endl;
            cleanupThreadRunning = false;
            return;
        }
        
        fs::path dllPath = fs::path(gameDirectoryForCleanup) / "XInput9_1_0.dll";
        
        std::cout << "Attempting to delete DLL at: " << dllPath.string() << std::endl;
        
        try {
            if (fs::exists(dllPath)) {
                std::cout << "DLL found, deleting: " << dllPath.string() << std::endl;
                
                for (int i = 0; i < 5; i++) {
                    try {
                        if (fs::remove(dllPath)) {
                            std::cout << "Successfully deleted DLL" << std::endl;
                            break;
                        } else {
                            std::cout << "Failed to delete DLL, attempt " << (i+1) << std::endl;
                            std::this_thread::sleep_for(std::chrono::seconds(1));
                        }
                    } catch (const std::exception& e) {
                        std::cout << "Error deleting DLL (attempt " << (i+1) << "): " << e.what() << std::endl;
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                    }
                }
            } else {
                std::cout << "DLL not found at: " << dllPath.string() << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Error accessing DLL path: " << e.what() << std::endl;
        }
        
        cleanupThreadRunning = false;
    }

    bool launchGame(const std::string& gameExePath, bool isOnline) {
        std::cout << "Launching game: " << gameExePath << (isOnline ? " -multiplayer" : " -zombies") << std::endl;
        
        fs::path gameDir = fs::path(gameExePath).parent_path();
        std::string gameDirStr = gameDir.string();
        
        gameDirectoryForCleanup = gameDirStr;
        
        std::string gameFlags = isOnline ? " -multiplayer" : " -zombies";
        
        SHELLEXECUTEINFOA shExInfo = {0};
        shExInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
        shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        shExInfo.hwnd = NULL;
        shExInfo.lpVerb = "open";
        shExInfo.lpFile = gameExePath.c_str();
        shExInfo.lpParameters = gameFlags.c_str();
        shExInfo.lpDirectory = gameDirStr.c_str();  
        shExInfo.nShow = SW_SHOW;
        
        std::cout << "Launching from directory: " << gameDirStr << std::endl;
        
        if (!ShellExecuteExA(&shExInfo)) {
            DWORD error = GetLastError();
            std::cout << "ShellExecuteEx failed with error: " << error << std::endl;
            return false;
        }
        
        if (shExInfo.hProcess) {
            CloseHandle(shExInfo.hProcess);
        }
        
        if (cleanupThreadRunning && cleanupThread.joinable()) {
            cleanupThread.join(); 
        }
        
        cleanupThread = std::thread(cleanupDllsAfterGame);
        cleanupThread.detach(); 
        
        std::cout << "Game launched successfully!" << std::endl;
        return true;
    }
}