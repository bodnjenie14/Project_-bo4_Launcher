import os

#Remove dll file otherwise it will be injected on start up
try:
    files_to_remove = ["d3d11.dll", "powrprof.dll"]

    for file_name in files_to_remove:
        file_path = os.path.join(os.getcwd(), file_name)

        if os.path.exists(file_path):
            os.remove(file_path)
except Exception as e:
    print(f"Error while removing dll files: {e}")

from PyQt5.QtWidgets import QApplication, QMessageBox, QLineEdit, QLabel, QVBoxLayout, QHBoxLayout, QPushButton, QWidget, QDialog, QStyle
from PyQt5.QtGui import QPixmap, QIcon
from PyQt5.QtCore import Qt, QUrl
import subprocess
import shutil
import random
import json
import sys

def get_json_item(json_path, spot, name):
    with open(json_path, "r") as json_file:
        data = json.load(json_file)
        print(data[spot][name])
    return data[spot][name]

class change_name(QDialog):
    def __init__(self, parent=None):
        super(change_name, self).__init__(parent)
        
        self.setWindowTitle("Set Name")
        self.setWindowIcon(self.style().standardIcon(getattr(QStyle, 'SP_MessageBoxInformation')))
        
        layout = QVBoxLayout()

        self.title = QLabel(f"Enter new name (Current name: {get_json_item('project-bo4.json', 'identity', 'name')})")
        layout.addWidget(self.title)

        self.input_field = QLineEdit()
        self.input_field.setPlaceholderText(f"Enter Name: ")
        
        input_field_width = 300
        self.input_field.setFixedWidth(input_field_width)
        layout.addWidget(self.input_field)

        ok_button = QPushButton("OK")
        ok_button.clicked.connect(self.accept)
        layout.addWidget(ok_button)

        self.setLayout(layout)

        self.setStyleSheet("""
            QDialog {
                background-color: #333;
                color: white;
            }
            QLabel {
                color: white;
            }
            QLineEdit {
                background-color: #555;
                color: white;
                border: 1px solid #444;
            }
            QPushButton {
                background-color: #555;
                color: white;
                border: 1px solid #444;
                border-radius: 4px;
            }
            QPushButton:hover {
                background-color: #666;
            }
        """)

def set_name():
    dialog = change_name()
    if dialog.exec_() == QDialog.Accepted:
        name = dialog.input_field.text()
        print(f"Entered Name: {name}")
        if name != "":
            replace_json_value("project-bo4.json", name, 'identity', 'name')

class change_ip(QDialog):
    def __init__(self, parent=None):
        super(change_ip, self).__init__(parent)
        
        self.setWindowTitle("Multiplayer")
        self.setWindowIcon(self.style().standardIcon(getattr(QStyle, 'SP_MessageBoxInformation')))
        
        layout = QVBoxLayout()
        ip_address = get_json_item('project-bo4.json', 'demonware', 'ipv4')
        if ip_address == "":
            ip_address = "None"

        self.title = QLabel(f"Enter server IP address (Current IP Address: {ip_address})")
        layout.addWidget(self.title)

        self.input_field = QLineEdit()
        self.input_field.setPlaceholderText("IP Address")
        
        input_field_width = 300
        self.input_field.setFixedWidth(input_field_width)
        layout.addWidget(self.input_field)

        ok_button = QPushButton("OK")
        ok_button.clicked.connect(self.accept)
        layout.addWidget(ok_button)

        self.setLayout(layout)

        self.setStyleSheet("""
            QDialog {
                background-color: #333;
                color: white;
            }
            QLabel {
                color: white;
            }
            QLineEdit {
                background-color: #555;
                color: white;
                border: 1px solid #444;
            }
            QPushButton {
                background-color: #555;
                color: white;
                border: 1px solid #444;
                border-radius: 4px;
            }
            QPushButton:hover {
                background-color: #666;
            }
        """)

def replace_json_value(json_file_path, value, spot, key):
    with open(json_file_path, 'r') as json_file:
        data = json.load(json_file)
        data[spot][key] = value

    with open(json_file_path, 'w') as json_file:
        json.dump(data, json_file, indent=4) 

def set_ip():
    dialog = change_ip()
    result = dialog.exec_()

    try:
        dialog.accepted.disconnect()
    except TypeError:
        pass

    if result == QDialog.Accepted:
        ip_address = dialog.input_field.text()

        if " " in ip_address:
            ip_address = ip_address.replace(" ", "")

        print(f"Entered IP Address: {ip_address}")
        if ip_address != "":
            replace_json_value("project-bo4.json", ip_address, 'demonware', 'ipv4')

    return result

def start_game(which):

    if which == "solo":
        try:
            if os.path.exists(os.getcwd() + "/files/solo/d3d11.dll"):
                shutil.copy(os.getcwd() + "/files/solo/d3d11.dll", os.getcwd())
        except Exception as e:
            print(f"Error while copying dll files: {e}")

    elif which == "multi":
        try:
            if os.path.exists(os.getcwd() + "/files/mp/d3d11.dll"):
                shutil.copy(os.getcwd() + "/files/mp/d3d11.dll", os.getcwd())
        except Exception as e:
            print(f"Error while copying dll files: {e}")

        ip_address = get_json_item('project-bo4.json', 'demonware', 'ipv4')
        if ip_address == "":
            click = set_ip()

            if click == 0:
                return

            if ip_address == "" and click == QDialog.Accepted:
                start_game("Multiplayer")
                return
            else:
                return

    try:
        window.hide()
        process = subprocess.Popen(["BlackOps4.exe"])
        process.wait()
        sys.exit()
    except Exception as e:
        print(f"Error: {e}")


class launcher(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()
        
    def initUI(self):
        
        layout = QVBoxLayout()
        try:

            image_folder = os.path.join(os.getcwd(), "files/images")
            image_files = [f for f in os.listdir(image_folder) if f.lower().endswith(( '.jpg', '.jpeg', '.gif', '.bmp'))]
            
            random_image = random.choice(image_files)
            
            image_path = os.path.join(image_folder, random_image)

            background_label = QLabel(self)
            background_image = QPixmap(image_path)
            background_label.setPixmap(background_image)
            background_label.setGeometry(0, 0, background_image.width(), background_image.height())
            self.setFixedSize(background_image.width(), background_image.height())
        except Exception as e:
            print(e)

        self.setWindowTitle('Project-BO4')
        self.setWindowIcon(QIcon('files/images/exe_icon_bo4.ico'))
        title_layout = QVBoxLayout()
        self.title = QLabel("")
        title_layout.addWidget(self.title)
        layout.addLayout(title_layout)

        buttons_layout = QHBoxLayout()

        self.name = QPushButton("Change Name")
        self.name.clicked.connect(set_name)
        self.name.setStyleSheet("background-color: #555; color: white;")
        buttons_layout.addWidget(self.name)

        self.change_ip = QPushButton("Change IP Address")
        self.change_ip.clicked.connect(set_ip)
        self.change_ip.setStyleSheet("background-color: #555; color: white;")
        buttons_layout.addWidget(self.change_ip)

        self.solo_button = QPushButton("Offline")
        self.solo_button.clicked.connect(lambda: start_game(which="solo"))
        self.solo_button.setStyleSheet("background-color: #555; color: white;")
        buttons_layout.addWidget(self.solo_button)

        self.Multiplayer = QPushButton("Online")
        self.Multiplayer.clicked.connect(lambda: start_game(which="multi"))
        self.Multiplayer.setStyleSheet("background-color: #555; color: white;")
        buttons_layout.addWidget(self.Multiplayer)

        layout.addLayout(buttons_layout)

        self.setLayout(layout)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = launcher()
    window.show()
    sys.exit(app.exec_())
