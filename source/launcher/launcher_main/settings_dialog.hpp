#pragma once
#include "std_include.hpp"
#include <string>
#include <rapidjson/document.h>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog();
    
    void setJsonPath(const std::string& path);

private slots:
    void saveSettings();
    void resetSettings();
    void showContextMenu(const QPoint& pos);

private:
    void loadSettings();
    void createLayout();
    void populateTreeWidget(const rapidjson::Value& jsonObj, QTreeWidgetItem* parentItem = nullptr, const QString& path = "");
    void saveJsonFromTreeWidget(rapidjson::Document& doc, rapidjson::Value& jsonObj, QTreeWidgetItem* item, rapidjson::Document::AllocatorType& allocator);
    
    void showMessageBox(QMessageBox::Icon icon, const QString& title, const QString& text);
    
    QTreeWidget* treeWidget;
    QPushButton* saveButton;
    QPushButton* resetButton;
    QPushButton* cancelButton;
    
    std::string jsonPath;
};
