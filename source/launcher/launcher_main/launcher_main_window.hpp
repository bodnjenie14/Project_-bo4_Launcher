#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QSlider>
#include <QProgressBar>
#include <QComboBox>
#include <string>

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

private:
    std::wstring launcherDir;
    std::wstring serverIpFile;
    bool reshadeEnabled;
    int volume;

    //gui
    QPushButton* onlineButton;
    QPushButton* offlineButton;
    QPushButton* nameButton;
    QPushButton* ipButton;
    QPushButton* discordButton;
    QPushButton* wikiButton;
    QCheckBox* reshadeCheckbox;
    QLabel* volumeLabel;
    QSlider* volumeSlider;
    QProgressBar* progressBar;
};