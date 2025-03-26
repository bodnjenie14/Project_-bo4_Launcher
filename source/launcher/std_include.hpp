#pragma once

#pragma warning(push)
#pragma warning(disable: 4100)
#pragma warning(disable: 4127)
#pragma warning(disable: 4244)
#pragma warning(disable: 4458)
#pragma warning(disable: 4702)
#pragma warning(disable: 4996)
#pragma warning(disable: 5054)
#pragma warning(disable: 6011)
#pragma warning(disable: 6297)
#pragma warning(disable: 6385)
#pragma warning(disable: 6386)
#pragma warning(disable: 6387)
#pragma warning(disable: 26110)
#pragma warning(disable: 26451)
#pragma warning(disable: 26444)
#pragma warning(disable: 26451)
#pragma warning(disable: 26489)
#pragma warning(disable: 26495)
#pragma warning(disable: 26498)
#pragma warning(disable: 26812)
#pragma warning(disable: 28020)



#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <MsHTML.h>
#include <MsHtmHst.h>
#include <ExDisp.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <corecrt_io.h>
#include <fcntl.h>
#include <shellapi.h>
#include <csetjmp>
#include <ShlObj.h>
#include <winternl.h>
#include <VersionHelpers.h>
#include <Psapi.h>
#include <urlmon.h>
#include <atlbase.h>
#include <iphlpapi.h>
#include <wincrypt.h>
#include <set>
#include <filesystem>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

// min and max is required by gdi, therefore NOMINMAX won't work
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include <map>
#include <atomic>
#include <vector>
#include <mutex>
#include <queue>
#include <regex>
#include <chrono>
#include <thread>
#include <fstream>
#include <iostream>
#include <utility>
#include <filesystem>
#include <functional>
#include <sstream>
#include <optional>
#include <unordered_set>
#include <variant>
#include <fstream>

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


#include <QMainWindow>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QSlider>
#include <QProgressBar>
#include <QComboBox>
#include <QMessageBox>

#include <QtWidgets/QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QStyledItemDelegate>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QProcess>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QDirIterator>
#include <QtCore/QCoreApplication>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QRegularExpression>
#include <QtCore/QVariant>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QReadWriteLock>
#include <QtCore/QDateTime>

#include <QHeaderView>
#include <QFileInfo>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMouseEvent>
#include <QDialog>



#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>




#pragma warning(pop)
#pragma warning(disable: 4100)

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "urlmon.lib" )
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Pdh.lib") //pc stats (cpu ram ect)


#pragma comment(lib, "bcrypt.lib") // libevent-arc4random
//#pragma comment(lib, "glog.lib") 



using namespace std::literals;



