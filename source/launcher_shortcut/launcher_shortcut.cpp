#include <windows.h>
#include <string>
#include <filesystem>
#include <iostream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    
    std::filesystem::path shortcutPath = exePath;
    
    std::filesystem::path baseDir = shortcutPath.parent_path();
    
    std::filesystem::path launcherPath = baseDir / "project-bo4" / "launcher" / "Shield_Launcher.exe";
    
    if (!std::filesystem::exists(launcherPath)) {
        MessageBoxA(NULL, 
            "Launcher executable not found. Please make sure this shortcut is in the correct location.", 
            "BO4 Launcher Error", 
            MB_ICONERROR | MB_OK);
        return 1;
    }
    
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    if (!CreateProcessA(
        launcherPath.string().c_str(),  // Path to executable
        NULL,                           // Command line arguments
        NULL,                           // Process security attributes
        NULL,                           // Thread security attributes
        FALSE,                          // Inherit handles
        0,                              // Creation flags
        NULL,                           // Environment
        baseDir.string().c_str(),       // Current directory
        &si,                            // Startup info
        &pi                             // Process information
    )) {
        char errorMsg[256];
        sprintf_s(errorMsg, "Failed to start launcher. Error code: %lu", GetLastError());
        MessageBoxA(NULL, errorMsg, "BO4 Launcher Error", MB_ICONERROR | MB_OK);
        return 1;
    }
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    return 0;
}
