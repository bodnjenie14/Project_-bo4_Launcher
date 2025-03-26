#pragma once

#include "std_include.hpp"
#include <string>
#include <Windows.h>
#pragma comment(lib, "winmm.lib")

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:

    void startGame(bool isOnline);
    void setName();
    void setIp();
    void openDiscord();
    void openDocs();
    void updateVolume(int value);
    void openSettings();

private:

    void showMessageBox(QMessageBox::Icon icon, const QString& title, const QString& text);
    bool copyLPCFolder();
    void playStartupSound();
    void saveVolumeSettings();
    void loadVolumeSettings();
    
    std::wstring launcherDir;
    std::wstring serverIpFile;
    bool reshadeEnabled;
    int volume;
    std::wstring soundPath;

    //gui
    QPushButton* onlineButton;
    QPushButton* offlineButton;
    QPushButton* nameButton;
    QPushButton* ipButton;
    QPushButton* discordButton;
    QPushButton* wikiButton;
    QPushButton* settingsButton;
    QCheckBox* reshadeCheckbox;
    QLabel* volumeLabel;
    QSlider* volumeSlider;
    QProgressBar* progressBar;
};