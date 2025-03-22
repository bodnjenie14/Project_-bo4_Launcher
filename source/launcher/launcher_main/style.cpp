#include "std_include.hpp"
#include "style.hpp"

//me love qt due to using css for design :D 
namespace Style {

    QString getLightStyleSheet() {
        return QString(R"(
            QMainWindow, QDialog {
                background-color: #f0f0f0;
                color: #333;
            }
            QLabel {
                color: #333;
            }
            QLineEdit {
                background-color: white;
                color: #333;
                border: 1px solid #ccc;
                padding: 4px;
                border-radius: 4px;
            }
            QPushButton {
                background-color: #e0e0e0;
                color: #333;
                border: 1px solid #ccc;
                border-radius: 4px;
                padding: 6px 12px;
                min-width: 80px;
            }
            QPushButton:hover {
                background-color: #d0d0d0;
            }
            QPushButton:pressed {
                background-color: #c0c0c0;
            }
            QComboBox {
                background-color: white;
                color: #333;
                border: 1px solid #ccc;
                border-radius: 4px;
                padding: 4px;
            }
            QComboBox::drop-down {
                border: none;
            }
            QComboBox::down-arrow {
                background-color: #ccc;
                width: 12px;
                height: 12px;
            }
            QComboBox QAbstractItemView {
                background-color: white;
                color: #333;
                selection-background-color: #e0e0e0;
            }
            QSlider::groove:horizontal {
                border: 1px solid #ccc;
                height: 8px;
                background: #e0e0e0;
                margin: 2px 0;
                border-radius: 4px;
            }
            QSlider::handle:horizontal {
                background: #999;
                border: 1px solid #ccc;
                width: 18px;
                margin: -2px 0;
                border-radius: 9px;
            }
            QSlider::handle:horizontal:hover {
                background: #888;
            }
            QCheckBox {
                color: #333;
            }
            QCheckBox::indicator {
                width: 16px;
                height: 16px;
            }
            QCheckBox::indicator:unchecked {
                background-color: white;
                border: 1px solid #ccc;
                border-radius: 3px;
            }
            QCheckBox::indicator:checked {
                background-color: #e0e0e0;
                border: 1px solid #ccc;
                border-radius: 3px;
            }
            QProgressBar {
                border: 1px solid #ccc;
                border-radius: 4px;
                background-color: white;
                color: #333;
                text-align: center;
            }
            QProgressBar::chunk {
                background-color: #999;
                width: 1px;
            }
            QMessageBox {
                background-color: #f0f0f0;
                color: #333;
            }
            QMessageBox QLabel {
                color: #333;
            }
        )");
    }

    QString getStyleSheet() {
        return getStyleSheet(true);  // Default to dark mode
    }

    QString getStyleSheet(bool darkMode) {
        return darkMode ? getDarkStyleSheet() : getLightStyleSheet();
    }

    QString getDarkStyleSheet() {
        return QString(R"(
            QMainWindow {
                background-color: transparent;
            }
            QWidget {
                background-color: transparent;
                color: white;
            }
            QPushButton {
                background-color: rgba(85, 85, 85, 180);
                color: white;
                border: none;
                border-radius: 4px;
                padding: 5px;
                min-width: 80px;
            }
            QPushButton:hover {
                background-color: rgba(102, 102, 102, 180);
            }
            QPushButton:pressed {
                background-color: rgba(119, 119, 119, 180);
            }
            QLabel {
                color: white;
            }
            QLineEdit {
                background-color: rgba(85, 85, 85, 180);
                color: white;
                border: none;
                border-radius: 4px;
                padding: 5px;
            }
            QProgressBar {
                border: 1px solid rgba(68, 68, 68, 180);
                border-radius: 4px;
                text-align: center;
                background: transparent;
            }
            QProgressBar::chunk {
                background-color: rgba(85, 85, 85, 180);
            }
            QSlider::groove:horizontal {
                border: 1px solid rgba(68, 68, 68, 180);
                height: 8px;
                background: rgba(85, 85, 85, 180);
                margin: 2px 0;
                border-radius: 4px;
            }
            QSlider::handle:horizontal {
                background: white;
                border: none;
                width: 18px;
                margin: -2px 0;
                border-radius: 9px;
            }
            QCheckBox {
                color: white;
                spacing: 5px;
            }
            QCheckBox::indicator {
                width: 15px;
                height: 15px;
            }
            QCheckBox::indicator:unchecked {
                border: 1px solid rgba(68, 68, 68, 180);
                background: rgba(85, 85, 85, 180);
            }
            QCheckBox::indicator:checked {
                border: 1px solid rgba(68, 68, 68, 180);
                background: rgba(119, 119, 119, 180);
            }
            QComboBox {
                background-color: rgba(85, 85, 85, 180);
                color: white;
                border: none;
                border-radius: 4px;
                padding: 5px;
            }
            QComboBox::drop-down {
                border: none;
            }
            QComboBox::down-arrow {
                background-color: white;
                width: 12px;
                height: 12px;
            }
            QComboBox QAbstractItemView {
                background-color: rgba(51, 51, 51, 180);
                color: white;
                selection-background-color: rgba(85, 85, 85, 180);
            }
            QMessageBox {
                background-color: rgba(51, 51, 51, 180);
                color: white;
            }
            QMessageBox QLabel {
                color: white;
            }
            QDialog {
                background-color: rgba(51, 51, 51, 180);
                color: white;
            }
        )");
    }
}
