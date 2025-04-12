#include "std_include.hpp"
#include "settings_dialog.hpp"
#include "../launcher_funcs/json_utils.hpp"
#include "window_utils.hpp"



namespace fs = std::filesystem;

class SettingsItemDelegate : public QStyledItemDelegate {
public:
    explicit SettingsItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        if (index.column() != 1) return nullptr;

        QString type = index.data(Qt::UserRole).toString();
        
        if (type == "bool") {
            QComboBox* comboBox = new QComboBox(parent);
            comboBox->addItem("Disable", false);
            comboBox->addItem("Enable", true);
            comboBox->setStyleSheet("QComboBox { background-color: #151515; color: white; border: 1px solid #333333; padding: 5px; }");
            return comboBox;
        } else if (type == "int") {
            QSpinBox* spinBox = new QSpinBox(parent);
            spinBox->setRange(-99999, 99999);
            spinBox->setStyleSheet("QSpinBox { background-color: #151515; color: white; border: 1px solid #333333; padding: 5px; selection-background-color: #1a3c5e; }");
            return spinBox;
        } else if (type == "string") {
            QLineEdit* lineEdit = new QLineEdit(parent);
            lineEdit->setStyleSheet("QLineEdit { background-color: #151515; color: white; border: 1px solid #333333; padding: 5px; selection-background-color: #1a3c5e; }");
            return lineEdit;
        }
        
        return nullptr;
    }

    void setEditorData(QWidget* editor, const QModelIndex& index) const override {
        QString type = index.data(Qt::UserRole).toString();
        
        if (type == "bool") {
            bool value = index.data(Qt::UserRole + 1).toBool();
            QComboBox* comboBox = dynamic_cast<QComboBox*>(editor);
            if (comboBox) {
                comboBox->setCurrentIndex(value ? 1 : 0);
            }
        } else if (type == "int") {
            int value = index.data(Qt::UserRole + 1).toInt();
            QSpinBox* spinBox = dynamic_cast<QSpinBox*>(editor);
            if (spinBox) {
                spinBox->setValue(value);
            }
        } else if (type == "string") {
            QString value = index.data(Qt::UserRole + 1).toString();
            QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
            if (lineEdit) {
                lineEdit->setText(value);
            }
        } else {
            QStyledItemDelegate::setEditorData(editor, index);
        }
    }

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override {
        QString type = index.data(Qt::UserRole).toString();
        
        if (type == "bool") {
            QComboBox* comboBox = dynamic_cast<QComboBox*>(editor);
            if (comboBox) {
                bool value = comboBox->currentData().toBool();
                model->setData(index, value ? "Enable" : "Disable", Qt::DisplayRole);
                model->setData(index, value, Qt::UserRole + 1);
            }
        } else if (type == "int") {
            QSpinBox* spinBox = dynamic_cast<QSpinBox*>(editor);
            if (spinBox) {
                int value = spinBox->value();
                model->setData(index, QString::number(value), Qt::DisplayRole);
                model->setData(index, value, Qt::UserRole + 1);
            }
        } else if (type == "string") {
            QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor);
            if (lineEdit) {
                QString value = lineEdit->text();
                model->setData(index, value, Qt::DisplayRole);
                model->setData(index, value, Qt::UserRole + 1);
            }
        } else {
            QStyledItemDelegate::setModelData(editor, model, index);
        }
    }

    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override {
        if (index.column() == 1) {
            QString type = index.data(Qt::UserRole).toString();
            
            if (type == "bool" && event->type() == QEvent::MouseButtonRelease) {
                bool currentValue = index.data(Qt::UserRole + 1).toBool();
                bool newValue = !currentValue;
                model->setData(index, newValue ? "Enable" : "Disable", Qt::DisplayRole);
                model->setData(index, newValue, Qt::UserRole + 1);
                return true;
            } else if (type == "string" && event->type() == QEvent::MouseButtonDblClick) {
                QTreeWidget* treeWidget = qobject_cast<QTreeWidget*>(const_cast<QWidget*>(option.widget));
                if (treeWidget) {
                    QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
                    if (mouseEvent) {
                        QTreeWidgetItem* item = treeWidget->itemAt(mouseEvent->pos());
                        if (item) {
                            QString currentValue = index.data(Qt::UserRole + 1).toString();
                            QString itemName = item->text(0);
                            
                            bool ok;
                            QString newValue = QInputDialog::getText(
                                treeWidget, 
                                "Edit " + itemName, 
                                "Enter value for " + itemName + ":", 
                                QLineEdit::Normal, 
                                currentValue, 
                                &ok
                            );
                            
                            if (ok) {
                                model->setData(index, newValue, Qt::DisplayRole);
                                model->setData(index, newValue, Qt::UserRole + 1);
                            }
                            return true;
                        }
                    }
                }
            }
        }
        
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }
};

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Settings");
    setMinimumSize(600, 500);
    
    WindowUtils::setWindowIcon(this);
    
    createLayout();
    
    connect(saveButton, &QPushButton::clicked, this, &SettingsDialog::saveSettings);
    connect(resetButton, &QPushButton::clicked, this, &SettingsDialog::resetSettings);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeWidget, &QTreeWidget::customContextMenuRequested, this, &SettingsDialog::showContextMenu);
    
    connect(treeWidget, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int column) {
        if (column == 1 && item->childCount() == 0) {
            treeWidget->editItem(item, column);
        }
    });
}

SettingsDialog::~SettingsDialog() {
}

void SettingsDialog::showContextMenu(const QPoint& pos) {
    QTreeWidgetItem* item = treeWidget->itemAt(pos);
    if (!item) return;
    
    QMenu contextMenu(this);
    
    if (item->childCount() == 0) {
        QAction* editAction = new QAction("Edit Value", this);
        connect(editAction, &QAction::triggered, this, [this, item]() {
            treeWidget->editItem(item, 1);
        });
        contextMenu.addAction(editAction);
    }
    
    if (!contextMenu.actions().isEmpty()) {
        contextMenu.exec(treeWidget->mapToGlobal(pos));
    }
}

void SettingsDialog::setJsonPath(const std::string& path) {
    jsonPath = path;
    loadSettings();
}

void SettingsDialog::createLayout() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    
    treeWidget = new QTreeWidget(this);
    treeWidget->setColumnCount(2);
    treeWidget->setHeaderLabels(QStringList() << "Setting" << "Value");
    treeWidget->setAlternatingRowColors(true);
    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    treeWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    treeWidget->setItemDelegate(new SettingsItemDelegate(this));
    treeWidget->setStyleSheet(
        "QTreeWidget {"
        "   background-color: #0a0a0a;"
        "   alternate-background-color: #101010;"
        "   color: white;"
        "   border: 1px solid #333333;"
        "}"
        "QTreeWidget::item {"
        "   padding: 5px;"
        "   border-bottom: 1px solid #222222;"
        "}"
        "QTreeWidget::item:selected {"
        "   background-color: #1a3c5e;"  
        "}"
        "QTreeWidget::branch {"
        "   background-color: transparent;"
        "}"
        "QHeaderView::section {"
        "   background-color: #151515;"
        "   color: #cccccc;"
        "   padding: 5px;"
        "   border: 1px solid #333333;"
        "}"
    );
    
    QHeaderView* header = treeWidget->header();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    
    connect(treeWidget, &QTreeWidget::customContextMenuRequested, this, &SettingsDialog::showContextMenu);
    
    mainLayout->addWidget(treeWidget);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);
    
    saveButton = new QPushButton("Save", this);
    resetButton = new QPushButton("Reset", this);
    cancelButton = new QPushButton("Cancel", this);
    
    QString buttonStyle = 
        "QPushButton {"
        "   background-color: #1a3c5e;"  
        "   color: white;"
        "   border: none;"
        "   padding: 8px 16px;"
        "   min-width: 100px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #2a5c8e;"  
        "}"
        "QPushButton:pressed {"
        "   background-color: #0a2c4e;"  
        "}";
    
    saveButton->setStyleSheet(buttonStyle);
    resetButton->setStyleSheet(buttonStyle);
    cancelButton->setStyleSheet(buttonStyle);
    
    connect(saveButton, &QPushButton::clicked, this, &SettingsDialog::saveSettings);
    connect(resetButton, &QPushButton::clicked, this, &SettingsDialog::resetSettings);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    setStyleSheet(
        "QDialog {"
        "   background-color: #0a0a0a;"
        "   color: white;"
        "}"
        "QLineEdit {"
        "   background-color: #151515;"
        "   color: white;"
        "   border: 1px solid #333333;"
        "   padding: 5px;"
        "   selection-background-color: #1a3c5e;"  
        "}"
        "QSpinBox {"
        "   background-color: #151515;"
        "   color: white;"
        "   border: 1px solid #333333;"
        "   padding: 5px;"
        "   selection-background-color: #1a3c5e;"  
        "}"
        "QCheckBox {"
        "   color: white;"
        "}"
        "QCheckBox::indicator {"
        "   width: 16px;"
        "   height: 16px;"
        "}"
        "QCheckBox::indicator:unchecked {"
        "   background-color: #151515;"
        "   border: 1px solid #333333;"
        "}"
        "QCheckBox::indicator:checked {"
        "   background-color: #1a3c5e;"  
        "   border: 1px solid #333333;"
        "}"
    );
}

void SettingsDialog::loadSettings() {
    treeWidget->clear();
    
    if (!fs::exists(jsonPath)) {
        showMessageBox(QMessageBox::Critical, "Error", "Could not find project-bo4.json file");
        return;
    }
    
    FILE* fp = nullptr;
    fopen_s(&fp, jsonPath.c_str(), "rb");
    if (!fp) {
        showMessageBox(QMessageBox::Critical, "Error", "Failed to open settings file");
        return;
    }
    
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);
    
    if (doc.HasParseError() || !doc.IsObject()) {
        showMessageBox(QMessageBox::Critical, "Error", "Invalid JSON format in settings file");
        return;
    }
    
    populateTreeWidget(doc);
    
    treeWidget->expandAll();
}

void SettingsDialog::populateTreeWidget(const rapidjson::Value& jsonObj, QTreeWidgetItem* parentItem, const QString& path) {
    for (auto it = jsonObj.MemberBegin(); it != jsonObj.MemberEnd(); ++it) {
        QString name = QString::fromUtf8(it->name.GetString());
        QString currentPath = path.isEmpty() ? name : path + "." + name;
        
        QTreeWidgetItem* item;
        if (parentItem) {
            item = new QTreeWidgetItem(parentItem);
        } else {
            item = new QTreeWidgetItem(treeWidget);
        }
        
        item->setText(0, name);
        item->setData(0, Qt::UserRole, currentPath);
        
        if (it->value.IsObject()) {
            item->setText(1, "");
            item->setData(1, Qt::UserRole, "object");
            
            item->setBackground(0, QColor(26, 60, 94)); 
            item->setBackground(1, QColor(26, 60, 94));
            item->setForeground(0, QColor(255, 255, 255)); 
            item->setForeground(1, QColor(255, 255, 255));
            
            QFont font = item->font(0);
            font.setBold(true);
            item->setFont(0, font);
            
            populateTreeWidget(it->value, item, currentPath);
        } else if (it->value.IsBool()) {
            bool value = it->value.GetBool();
            
            item->setText(1, value ? "Enable" : "Disable");
            item->setData(1, Qt::UserRole, "bool");
            item->setData(1, Qt::UserRole + 1, value);        
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            
            if (value) {
                item->setBackground(1, QColor(26, 60, 94, 128));
            } else {
                item->setBackground(1, QColor(40, 40, 40, 128));
            }
        } else if (it->value.IsInt()) {
            int value = it->value.GetInt();
            item->setText(1, QString::number(value));
            item->setData(1, Qt::UserRole, "int");
            item->setData(1, Qt::UserRole + 1, value);
            
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        } else if (it->value.IsString()) {
            QString value = QString::fromUtf8(it->value.GetString());
            item->setText(1, value);
            item->setData(1, Qt::UserRole, "string");
            item->setData(1, Qt::UserRole + 1, value);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            
            if (name == "name" && path == "identity") {
                item->setBackground(0, QColor(26, 60, 94)); 
                item->setBackground(1, QColor(26, 60, 94));
                item->setForeground(0, QColor(255, 255, 255)); 
                item->setForeground(1, QColor(255, 255, 255));
            }
        } else if (it->value.IsArray()) {
            item->setText(1, "[Array]");
            item->setData(1, Qt::UserRole, "array");
            item->setForeground(1, QColor(150, 150, 150));
        } else if (it->value.IsNull()) {
            item->setText(1, "null");
            item->setData(1, Qt::UserRole, "null");
            item->setForeground(1, QColor(150, 150, 150));
        } else {
            item->setText(1, "[Unknown Type]");
            item->setData(1, Qt::UserRole, "unknown");
            item->setForeground(1, QColor(150, 150, 150));
        }
    }
}

void SettingsDialog::saveSettings() {
    rapidjson::Document doc;
    doc.SetObject();
    
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = treeWidget->topLevelItem(i);
        QString name = item->text(0);
        
        rapidjson::Value jsonKey(name.toUtf8().constData(), doc.GetAllocator());
        rapidjson::Value jsonValue;
        
        if (item->childCount() > 0) {
            jsonValue.SetObject();
            saveJsonFromTreeWidget(doc, jsonValue, item, doc.GetAllocator());
        } else {
            QString type = item->data(1, Qt::UserRole).toString();
            
            if (type == "bool") {
                jsonValue.SetBool(item->data(1, Qt::UserRole + 1).toBool());
            } else if (type == "int") {
                jsonValue.SetInt(item->text(1).toInt());
            } else if (type == "double") {
                jsonValue.SetDouble(item->text(1).toDouble());
            } else if (type == "string") {
                std::string str = item->text(1).toStdString();
                jsonValue.SetString(str.c_str(), doc.GetAllocator());
            }
        }
        
        doc.AddMember(jsonKey, jsonValue, doc.GetAllocator());
    }
    
    FILE* fp = nullptr;
    fopen_s(&fp, jsonPath.c_str(), "wb");
    if (!fp) {
        showMessageBox(QMessageBox::Critical, "Error", "Failed to save settings file");
        return;
    }
    
    char writeBuffer[65536];
    rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
    doc.Accept(writer);
    fclose(fp);
    
    showMessageBox(QMessageBox::Information, "Success", "Settings saved successfully");
    accept();
}

void SettingsDialog::saveJsonFromTreeWidget(rapidjson::Document& doc, rapidjson::Value& jsonObj, QTreeWidgetItem* item, rapidjson::Document::AllocatorType& allocator) {
    for (int i = 0; i < item->childCount(); ++i) {
        QTreeWidgetItem* childItem = item->child(i);
        QString name = childItem->text(0);
        
        rapidjson::Value jsonKey(name.toUtf8().constData(), allocator);
        rapidjson::Value jsonValue;
        
        if (childItem->childCount() > 0) {
            jsonValue.SetObject();
            saveJsonFromTreeWidget(doc, jsonValue, childItem, allocator);
        } else {
            QString type = childItem->data(1, Qt::UserRole).toString();
            
            if (type == "bool") {
                jsonValue.SetBool(childItem->data(1, Qt::UserRole + 1).toBool());
            } else if (type == "int") {
                jsonValue.SetInt(childItem->text(1).toInt());
            } else if (type == "double") {
                jsonValue.SetDouble(childItem->text(1).toDouble());
            } else if (type == "string") {
                std::string str = childItem->text(1).toStdString();
                jsonValue.SetString(str.c_str(), allocator);
            }
        }
        
        jsonObj.AddMember(jsonKey, jsonValue, allocator);
    }
}

void SettingsDialog::resetSettings() {
    QMessageBox msgBox(QMessageBox::Question, "Reset Settings", "Are you sure you want to reset all settings to default?", 
                      QMessageBox::Yes | QMessageBox::No, this);
    WindowUtils::setWindowIcon(&msgBox);
    
    if (msgBox.exec() == QMessageBox::Yes) {
        JsonUtils::createDefaultJson(jsonPath);
        loadSettings();
    }
}

void SettingsDialog::showMessageBox(QMessageBox::Icon icon, const QString& title, const QString& text) {
    QMessageBox msgBox(icon, title, text, QMessageBox::Ok, this);
    WindowUtils::setWindowIcon(&msgBox);
    msgBox.exec();
}
