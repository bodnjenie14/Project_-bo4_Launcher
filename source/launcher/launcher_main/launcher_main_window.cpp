#include "std_include.hpp"
#include "launcher_main_window.hpp"
#include "style.hpp"
#include "../launcher_funcs/dll_loading.hpp"
#include "../launcher_funcs/json_utils.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QIcon>
#include <QDesktopServices>
#include <QUrl>
#include <QLineEdit>
#include <QTimer>
#include <QSpacerItem>
#include <QSizePolicy>
#include <set>
#include <filesystem>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
launcherDir(QCoreApplication::applicationDirPath().toStdWString()),  // Get exe directory
serverIpFile((fs::path(launcherDir) / L"servers.txt").wstring()),  // Properly construct path and convert to wstring
reshadeEnabled(false),
volume(100),
onlineButton(nullptr),
offlineButton(nullptr),
nameButton(nullptr),
ipButton(nullptr),
discordButton(nullptr),
wikiButton(nullptr),
reshadeCheckbox(nullptr),
volumeLabel(nullptr),
volumeSlider(nullptr),
progressBar(nullptr)
{
    fs::create_directories(launcherDir);

    setWindowTitle("Project-BO4");
    setFixedSize(800, 600);

    // Set window icon
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
    
    fs::path iconPath = gamePath / "project-bo4" / "launcher" / "images" / "icon.png";
    if (fs::exists(iconPath)) {
        QIcon icon(QString::fromStdString(iconPath.string()));
        setWindowIcon(icon);
        // Also set as application icon
        QApplication::setWindowIcon(icon);
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

    // Add spacer to push Docs button to the right
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
    volumeLabel->setStyleSheet("color: white; background: transparent;");
    settingsLayout->addWidget(volumeLabel);

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(volume);
    volumeSlider->setFixedWidth(100);
    volumeSlider->setStyleSheet("QSlider::groove:horizontal { background: rgba(85, 85, 85, 180); } QSlider::handle:horizontal { background: white; }");
    settingsLayout->addWidget(volumeSlider);

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

    //signals
    connect(onlineButton, &QPushButton::clicked, this, [this]() { startGame(true); });
    connect(offlineButton, &QPushButton::clicked, this, [this]() { startGame(false); });
    connect(nameButton, &QPushButton::clicked, this, &MainWindow::setName);
    connect(ipButton, &QPushButton::clicked, this, &MainWindow::setIp);
    connect(discordButton, &QPushButton::clicked, this, &MainWindow::openDiscord);
    connect(wikiButton, &QPushButton::clicked, this, &MainWindow::openDocs);
    connect(volumeSlider, &QSlider::valueChanged, this, &MainWindow::updateVolume);
    connect(reshadeCheckbox, &QCheckBox::stateChanged, this, [this](int state) {
        reshadeEnabled = (state == Qt::Checked);
    });

    setAttribute(Qt::WA_TranslucentBackground);
}

void MainWindow::startGame(bool isOnline) {
    progressBar->setVisible(true);
    progressBar->setValue(0);

    if (DllLoading::isGameRunning()) {
        QMessageBox::warning(this, "Warning", "Game is already running!");
        progressBar->setVisible(false);
        return;
    }

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
        QMessageBox::critical(this, "Error", errorMsg);
        progressBar->setVisible(false);
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
        QMessageBox::critical(this, "Error", "Could not locate BlackOps4.exe!");
        progressBar->setVisible(false);
        return;
    }
    
    if (!DllLoading::launchGame(gameExePath.string(), isOnline)) {
        QMessageBox::critical(this, "Error", "Failed to start the game!");
        progressBar->setVisible(false);
        return;
    }

    progressBar->setValue(100);
    progressBar->setVisible(false);
}

void MainWindow::setName() {
    QDialog dialog(this);
    dialog.setWindowTitle("Set Name");
    dialog.setFixedWidth(400);
    
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
            QMessageBox::warning(this, "Warning", "Name cannot be empty!");
            return;
        }

        if (!JsonUtils::replaceJsonValue(jsonPath, newName, "identity", "name")) {
            QMessageBox::critical(this, "Error", "Failed to save name!");
        }
    }
}

void MainWindow::setIp() {
    QDialog dialog(this);
    dialog.setWindowTitle("Set Server IP");
    dialog.setFixedWidth(400);
    
    // Apply the dark style sheet
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
            QMessageBox::warning(this, "Warning", "IP address cannot be empty!");
            return;
        }

        if (!JsonUtils::replaceJsonValue(jsonPath, newIp, "demonware", "ipv4")) {
            QMessageBox::critical(this, "Error", "Failed to save IP address!");
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
    volume = value;
    volumeLabel->setText(QString("Volume: %1%").arg(value));

    HRESULT hr;
    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);
    if (FAILED(hr)) return;

    IMMDevice* defaultDevice = nullptr;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    if (FAILED(hr)) return;

    IAudioEndpointVolume* endpointVolume = nullptr;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER,
        nullptr, (LPVOID*)&endpointVolume);
    defaultDevice->Release();
    if (FAILED(hr)) return;

    float newVolume = value / 100.0f;
    endpointVolume->SetMasterVolumeLevelScalar(newVolume, nullptr);
    endpointVolume->Release();
}