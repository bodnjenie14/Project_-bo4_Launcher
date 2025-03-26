#pragma once

#include <QWidget>
#include <QIcon>
#include <Windows.h>
#include <filesystem>
#include <QCoreApplication>

namespace fs = std::filesystem;

class WindowUtils {
public:
    static void setWindowIcon(QWidget* window) {
        if (!window) return;
        
        HINSTANCE hInstance = GetModuleHandle(NULL);
        HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
        
        if (hIcon) {
            SendMessage((HWND)window->winId(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
            SendMessage((HWND)window->winId(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        } else {
            std::wstring launcherDir = QCoreApplication::applicationDirPath().toStdWString();
            fs::path iconPath = fs::path(launcherDir) / "images" / "icon.ico";
            if (fs::exists(iconPath)) {
                HICON fileIcon = (HICON)LoadImageW(
                    NULL,
                    std::wstring(iconPath.wstring()).c_str(),
                    IMAGE_ICON,
                    0, 0,
                    LR_LOADFROMFILE
                );
                
                if (fileIcon) {
                    SendMessage((HWND)window->winId(), WM_SETICON, ICON_SMALL, (LPARAM)fileIcon);
                    SendMessage((HWND)window->winId(), WM_SETICON, ICON_BIG, (LPARAM)fileIcon);
                }
                
                QIcon icon(QString::fromStdString(iconPath.string()));
                if (!icon.isNull()) {
                    window->setWindowIcon(icon);
                }
            }
        }
    }
};
