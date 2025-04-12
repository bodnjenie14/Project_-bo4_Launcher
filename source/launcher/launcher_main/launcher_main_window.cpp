#include "std_include.hpp"
#include "launcher_main_window.hpp"
#include "style.hpp"
#include "settings_dialog.hpp"
#include "window_utils.hpp"
#include "utils.hpp"
#include "../launcher_funcs/dll_loading.hpp"
#include "../launcher_funcs/json_utils.hpp"
#include "../launcher_funcs/auto_update.hpp"
#include <mmsystem.h>

namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
launcherDir(QCoreApplication::applicationDirPath().toStdWString()),  
serverIpFile((fs::path(launcherDir) / L"servers.txt").wstring()),  
reshadeEnabled(false),
volume(100),
soundPath((fs::path(launcherDir) / L"sounds" / L"startup_sound.mp3").wstring()),
onlineButton(nullptr),
offlineButton(nullptr),
nameButton(nullptr),
ipButton(nullptr),
discordButton(nullptr),
wikiButton(nullptr),
settingsButton(nullptr),
reshadeCheckbox(nullptr),
volumeLabel(nullptr),
volumeSlider(nullptr),
progressBar(nullptr)
{
    fs::create_directories(launcherDir);
    fs::create_directories(fs::path(launcherDir) / "sounds");

    // Load volume settings from JSON
    loadVolumeSettings();

    setWindowTitle("Project-BO4");
    setFixedSize(800, 600);


    HINSTANCE hInstance = GetModuleHandle(NULL);
    HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));

    if (hIcon) {
        SendMessage((HWND)winId(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        SendMessage((HWND)winId(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    }
    else {
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
                SendMessage((HWND)winId(), WM_SETICON, ICON_SMALL, (LPARAM)fileIcon);
                SendMessage((HWND)winId(), WM_SETICON, ICON_BIG, (LPARAM)fileIcon);
            }

            QIcon icon(QString::fromStdString(iconPath.string()));
            if (!icon.isNull()) {
                setWindowIcon(icon);
                QApplication::setWindowIcon(icon);
            }
        }
    }

    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background: transparent;");

    //background label and set it as central widget
    QLabel* backgroundLabel = new QLabel(this);
    backgroundLabel->setFixedSize(800, 600);
    backgroundLabel->move(0, 0);

    //background image relative to exe location
    fs::path imagePath = fs::path(launcherDir) / "images" / "launcher.png";
    if (fs::exists(imagePath)) {
        QPixmap background(QString::fromStdString(imagePath.string()));
        if (!background.isNull()) {
            background = background.scaled(800, 600, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            backgroundLabel->setPixmap(background);
            backgroundLabel->setAlignment(Qt::AlignCenter);
        }
    }

    QWidget* container = new QWidget(this);
    container->setFixedSize(800, 600);
    container->setAttribute(Qt::WA_TranslucentBackground);
    container->setStyleSheet("background: transparent;");
    setCentralWidget(container);

    //mainlayout for all widgets
    QVBoxLayout* mainLayout = new QVBoxLayout(container);

    QHBoxLayout* topRowLayout = new QHBoxLayout();

    discordButton = new QPushButton("Discord", this);
    discordButton->setStyleSheet("background-color: rgba(85, 85, 85, 180); color: white; border: none; padding: 5px;");
    topRowLayout->addWidget(discordButton);

    //spacer to push Docs button to the right
    QSpacerItem* spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    topRowLayout->addItem(spacer);

    //docs
    wikiButton = new QPushButton("Shield Docs", this);
    wikiButton->setStyleSheet("background-color: rgba(85, 85, 85, 180); color: white; border: none; padding: 5px;");
    topRowLayout->addWidget(wikiButton);

    mainLayout->addLayout(topRowLayout);

    mainLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    //settings layout
    QHBoxLayout* settingsLayout = new QHBoxLayout();

    //volume slider
    volumeLabel = new QLabel("Volume:", this);
    volumeLabel->setStyleSheet("color: white; background: transparent; font-weight: bold;");
    settingsLayout->addWidget(volumeLabel);

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(volume);
    volumeSlider->setFixedWidth(120);
    volumeSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "  height: 6px;"
        "  background: rgba(40, 40, 40, 180);"
        "  border-radius: 3px;"
        "}"
        "QSlider::handle:horizontal {"
        "  background: #1a3c5e;"
        "  border: 1px solid #ffffff;"
        "  width: 14px;"
        "  margin-top: -4px;"
        "  margin-bottom: -4px;"
        "  border-radius: 7px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "  background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #1a3c5e, stop: 1 #2a5c8e);"
        "  border-radius: 3px;"
        "}"
    );
    settingsLayout->addWidget(volumeSlider);

    //spacer to push settings button to the right
    QSpacerItem* settingsSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    settingsLayout->addItem(settingsSpacer);

    //settings button 
    settingsButton = new QPushButton("Settings", this);
    settingsButton->setStyleSheet("background-color: rgba(85, 85, 85, 180); color: white; border: none; padding: 5px; min-width: 80px;");
    settingsButton->setFixedWidth(120);
    settingsLayout->addWidget(settingsButton);

    mainLayout->addLayout(settingsLayout);

    //button layout
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    //username and IP buttons
    nameButton = new QPushButton("Change Name", this);
    nameButton->setStyleSheet("background-color: rgba(85, 85, 85, 180); color: white; border: none; padding: 5px;");
    buttonLayout->addWidget(nameButton);

    ipButton = new QPushButton("Change IP Address", this);
    ipButton->setStyleSheet("background-color: rgba(85, 85, 85, 180); color: white; border: none; padding: 5px;");
    buttonLayout->addWidget(ipButton);

    //game mode buttons
    offlineButton = new QPushButton("Offline", this);
    offlineButton->setStyleSheet("background-color: rgba(85, 85, 85, 180); color: white; border: none; padding: 5px;");
    buttonLayout->addWidget(offlineButton);

    onlineButton = new QPushButton("Online", this);
    onlineButton->setStyleSheet("background-color: rgba(85, 85, 85, 180); color: white; border: none; padding: 5px;");
    buttonLayout->addWidget(onlineButton);

    mainLayout->addLayout(buttonLayout);

    //progress bar
    progressBar = new QProgressBar(this);
    progressBar->setStyleSheet("QProgressBar { border: 1px solid rgba(68, 68, 68, 180); text-align: center; background: transparent; } QProgressBar::chunk { background-color: rgba(85, 85, 85, 180); }");
    progressBar->setVisible(false);
    mainLayout->addWidget(progressBar);

    QTimer::singleShot(500, this, [this]() {
        updater::check_and_prompt_for_updates(this);
        QTimer::singleShot(1000, this, &MainWindow::playStartupSound);
        });

    //signals
    connect(onlineButton, &QPushButton::clicked, this, [this]() { startGame(true); });
    connect(offlineButton, &QPushButton::clicked, this, [this]() { startGame(false); });
    connect(nameButton, &QPushButton::clicked, this, &MainWindow::setName);
    connect(ipButton, &QPushButton::clicked, this, &MainWindow::setIp);
    connect(discordButton, &QPushButton::clicked, this, &MainWindow::openDiscord);
    connect(wikiButton, &QPushButton::clicked, this, &MainWindow::openDocs);
    connect(volumeSlider, &QSlider::valueChanged, this, &MainWindow::updateVolume);
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(reshadeCheckbox, &QCheckBox::stateChanged, this, [this](int state) {
        reshadeEnabled = (state == Qt::Checked);
        });

    setAttribute(Qt::WA_TranslucentBackground);
}

void MainWindow::startGame(bool isOnline) {

    onlineButton->setEnabled(false);
    offlineButton->setEnabled(false);

    progressBar->setVisible(true);
    progressBar->setValue(0);

    if (DllLoading::isGameRunning()) {
        showMessageBox(QMessageBox::Warning, "Warning", "Game is already running!");
        progressBar->setVisible(false);

        onlineButton->setEnabled(true);
        offlineButton->setEnabled(true);
        return;
    }

    progressBar->setValue(15);

    copyLPCFolder();

    progressBar->setValue(25);

    std::string currentDir = QCoreApplication::applicationDirPath().toStdString();

    auto result = DllLoading::extractDlls(currentDir, isOnline, reshadeEnabled);
    if (result != DllLoading::Result::Success) {
        QString errorMsg;
        switch (result) {
        case DllLoading::Result::FileNotFound:
            errorMsg = QString("Required files not found! Make sure %1 exists in the launcher directory.")
                .arg(isOnline ? "mp.zip" : "solo.zip");
            break;
        case DllLoading::Result::InvalidGamePath:
            errorMsg = "BlackOps4.exe not found! Make sure the launcher shortcut is in the game directory.";
            break;
        case DllLoading::Result::ZipError:
            errorMsg = "Failed to extract files!";
            break;
        default:
            errorMsg = "Unknown error occurred while extracting files.";
        }
        showMessageBox(QMessageBox::Critical, "Error", errorMsg);
        progressBar->setVisible(false);

        onlineButton->setEnabled(true);
        offlineButton->setEnabled(true);
        return;
    }

    progressBar->setValue(75);

    fs::path currentPath = fs::path(currentDir);
    fs::path gameExePath;

    if (fs::exists(currentPath / "BlackOps4.exe")) {
        gameExePath = currentPath / "BlackOps4.exe";
    }
    else if (fs::exists(currentPath.parent_path().parent_path() / "BlackOps4.exe")) {
        gameExePath = currentPath.parent_path().parent_path() / "BlackOps4.exe";
    }
    else {
        showMessageBox(QMessageBox::Critical, "Error", "Could not locate BlackOps4.exe!");
        progressBar->setVisible(false);

        onlineButton->setEnabled(true);
        offlineButton->setEnabled(true);
        return;
    }

    if (!DllLoading::launchGame(gameExePath.string(), isOnline)) {
        showMessageBox(QMessageBox::Critical, "Error", "Failed to start the game!");
        progressBar->setVisible(false);

        onlineButton->setEnabled(true);
        offlineButton->setEnabled(true);
        return;
    }

    progressBar->setValue(100);
    progressBar->setVisible(false);

    onlineButton->setEnabled(true);
    offlineButton->setEnabled(true);
}

void MainWindow::setName() {

    QDialog dialog(this);
    dialog.setWindowTitle("Set Name");
    dialog.setFixedWidth(400);

    WindowUtils::setWindowIcon(&dialog);

    dialog.setStyleSheet(Style::getDarkStyleSheet());

    QVBoxLayout layout(&dialog);

    std::string currentDir = QCoreApplication::applicationDirPath().toStdString();
    fs::path gamePath = fs::path(currentDir);
    fs::path gameExePath = gamePath / "BlackOps4.exe";

    if (!fs::exists(gameExePath)) {

        fs::path parentPath = gamePath.parent_path().parent_path();
        gameExePath = parentPath / "BlackOps4.exe";

        if (fs::exists(gameExePath)) {
            gamePath = parentPath;
        }
    }

    std::string jsonPath = (gamePath / "project-bo4.json").string();
    std::string currentName = JsonUtils::getJsonItem(jsonPath, "identity", "name");

    QLabel label("Enter your name:");
    layout.addWidget(&label);

    QLineEdit lineEdit;
    lineEdit.setText(QString::fromStdString(currentName));
    layout.addWidget(&lineEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        std::string newName = lineEdit.text().toStdString();
        if (newName.empty()) {
            QMessageBox msgBox(QMessageBox::Warning, "Warning", "Name cannot be empty!", QMessageBox::Ok, this);
            WindowUtils::setWindowIcon(&msgBox);
            msgBox.exec();
            return;
        }

        if (!JsonUtils::replaceJsonValue(jsonPath, newName, "identity", "name")) {
            QMessageBox msgBox(QMessageBox::Critical, "Error", "Failed to save name!", QMessageBox::Ok, this);
            WindowUtils::setWindowIcon(&msgBox);
            msgBox.exec();
        }
    }
}

void MainWindow::setIp() {

    QDialog dialog(this);
    dialog.setWindowTitle("Set Server IP");
    dialog.setFixedWidth(400);

    WindowUtils::setWindowIcon(&dialog);

    dialog.setStyleSheet(Style::getDarkStyleSheet());

    QVBoxLayout layout(&dialog);

    std::string currentDir = QCoreApplication::applicationDirPath().toStdString();
    fs::path gamePath = fs::path(currentDir);
    fs::path gameExePath = gamePath / "BlackOps4.exe";

    if (!fs::exists(gameExePath)) {
        fs::path parentPath = gamePath.parent_path().parent_path();
        gameExePath = parentPath / "BlackOps4.exe";

        if (fs::exists(gameExePath)) {
            gamePath = parentPath;
        }
    }

    std::string jsonPath = (gamePath / "project-bo4.json").string();
    std::string currentIp = JsonUtils::getJsonItem(jsonPath, "demonware", "ipv4");

    QLabel label("Enter server IP address:");
    layout.addWidget(&label);

    QLineEdit lineEdit;
    lineEdit.setText(QString::fromStdString(currentIp));
    layout.addWidget(&lineEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        std::string newIp = lineEdit.text().toStdString();
        if (newIp.empty()) {
            QMessageBox msgBox(QMessageBox::Warning, "Warning", "IP address cannot be empty!", QMessageBox::Ok, this);
            WindowUtils::setWindowIcon(&msgBox);
            msgBox.exec();
            return;
        }

        if (!JsonUtils::replaceJsonValue(jsonPath, newIp, "demonware", "ipv4")) {
            QMessageBox msgBox(QMessageBox::Critical, "Error", "Failed to save IP address!", QMessageBox::Ok, this);
            WindowUtils::setWindowIcon(&msgBox);
            msgBox.exec();
        }
    }
}

void MainWindow::openDiscord() {

    QDesktopServices::openUrl(QUrl("https://discord.gg/AXECAzJJGU"));
}

void MainWindow::openDocs() {

    QDesktopServices::openUrl(QUrl("https://shield-bo4.gitbook.io/"));
}

void MainWindow::updateVolume(int value) {
    // Only update the internal volume variable, don't change system volume
    volume = value;
    volumeLabel->setText(QString("Volume: %1%").arg(value));
    
    // If the MP3 is currently playing, update its volume
    std::wstring volumeCmd = L"setaudio mp3 volume to " + std::to_wstring(value * 10);
    mciSendStringW(volumeCmd.c_str(), NULL, 0, NULL);
    
    // Save the volume setting
    saveVolumeSettings();
}

void MainWindow::openSettings() {

    fs::path gameDir = fs::path(launcherDir).parent_path().parent_path();
    std::string jsonPath = (gameDir / "project-bo4.json").string();

    SettingsDialog settingsDialog(this);
    settingsDialog.setJsonPath(jsonPath);
    settingsDialog.exec();
}

void MainWindow::playStartupSound() {

    if (!fs::exists(soundPath)) {
        QMessageBox::information(this, "Debug", "Sound file not found at: " + QString::fromStdWString(soundPath));
        return;
    }

    // Close any existing MP3 playback
    std::wstring closeCmd = L"close mp3";
    mciSendStringW(closeCmd.c_str(), NULL, 0, NULL);

    // Open the MP3 file
    std::wstring openCmd = L"open \"" + soundPath + L"\" type mpegvideo alias mp3";
    MCIERROR openError = mciSendStringW(openCmd.c_str(), NULL, 0, NULL);
    
    if (openError != 0) {
        wchar_t errorText[256];
        mciGetErrorStringW(openError, errorText, 256);
        QMessageBox::warning(this, "Sound Error", "Failed to open sound file: " + 
                            QString::fromWCharArray(errorText));
        return;
    }
    
    // Set the volume for the MP3 (volume range for MCI is 0-1000, our volume is 0-100)
    std::wstring volumeCmd = L"setaudio mp3 volume to " + std::to_wstring(volume * 10);
    mciSendStringW(volumeCmd.c_str(), NULL, 0, NULL);

    // Play the MP3
    std::wstring playCmd = L"play mp3";
    MCIERROR playError = mciSendStringW(playCmd.c_str(), NULL, 0, NULL);
    
    if (playError != 0) {
        wchar_t errorText[256];
        mciGetErrorStringW(playError, errorText, 256);
        QMessageBox::warning(this, "Sound Error", "Failed to play sound file: " + 
                            QString::fromWCharArray(errorText));
    }
}

void MainWindow::showMessageBox(QMessageBox::Icon icon, const QString& title, const QString& text) {

    QMessageBox msgBox(this);
    msgBox.setIcon(icon);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::saveVolumeSettings() {
    try {

        std::string jsonPath = (fs::path(launcherDir) / "launcher-config.json").string();

        rapidjson::Document document;
        if (fs::exists(jsonPath)) {
            std::ifstream file(jsonPath);
            std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            if (!jsonContent.empty()) {
                document.Parse(jsonContent.c_str());
            }
        }

        if (!document.IsObject()) {
            document.SetObject();
        }

        if (!document.HasMember("audio")) {
            rapidjson::Value audioObject(rapidjson::kObjectType);
            document.AddMember("audio", audioObject, document.GetAllocator());
        }

        rapidjson::Value& audioObject = document["audio"];
        if (audioObject.HasMember("volume")) {
            audioObject["volume"] = volume;
        }
        else {
            rapidjson::Value volumeValue(volume);
            audioObject.AddMember("volume", volumeValue, document.GetAllocator());
        }

        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);

        std::ofstream outFile(jsonPath);
        outFile << buffer.GetString();
        outFile.close();
    }
    catch (const std::exception& e) {
        
    }
}

void MainWindow::loadVolumeSettings() {
    try {
        std::string jsonPath = (fs::path(launcherDir) / "launcher-config.json").string();

        if (fs::exists(jsonPath)) {
            std::ifstream file(jsonPath);
            std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            if (!jsonContent.empty()) {
                rapidjson::Document document;
                document.Parse(jsonContent.c_str());

                if (document.IsObject() && document.HasMember("audio") && 
                    document["audio"].IsObject() && document["audio"].HasMember("volume") && 
                    document["audio"]["volume"].IsInt()) {
                    volume = document["audio"]["volume"].GetInt();
                    
                    if (volume < 0) volume = 0;
                    if (volume > 100) volume = 100;
                    
                    if (volumeSlider) {
                        volumeSlider->setValue(volume);
                    }
                }
            }
        }
    }
    catch (const std::exception& e) {
    }
}

bool MainWindow::copyLPCFolder() {

    std::string currentDir = QCoreApplication::applicationDirPath().toStdString();
    fs::path currentPath = fs::path(currentDir);
    fs::path sourceLPCPath = currentPath / "LPC";
    fs::path gameDir = currentPath.parent_path().parent_path();
    fs::path destLPCPath = gameDir / "LPC";

    if (!fs::exists(sourceLPCPath)) {
        return false;
    }

    return utils::copyDirectoryRecursive(sourceLPCPath.string(), destLPCPath.string());
}