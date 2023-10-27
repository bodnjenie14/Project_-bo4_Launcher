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

from PyQt5.QtWidgets import QApplication, QMessageBox, QLineEdit, QLabel, QVBoxLayout, QHBoxLayout, QPushButton, QWidget, QDialog, QStyle, QCheckBox
from PyQt5.QtMultimedia import QMediaPlayer, QMediaContent
from PyQt5.QtGui import QPixmap, QIcon, QMovie, QPalette
from PyQt5.QtMultimediaWidgets import QVideoWidget
from PyQt5.QtCore import Qt, QUrl, QTimer
import subprocess
import shutil
import random
import json
import sys

global reshade
reshade = False

def get_json_item(json_path, spot, name):
    try:
        if os.path.exists(json_path):
            with open(json_path, "r") as json_file:
                data = json.load(json_file)
                print(data[spot][name])
        else:
            print("Creating default json")

            data = {
                "demonware":
                {
                    "ipv4": "127.0.0.1"
                },
                "identity":
                {
                    "name":  f'{os.getlogin()}',
                }
            }
            json_data = json.dumps(data, indent=4) 

            with open(json_path, "w") as json_file:
                json_file.write(json_data)

            return get_json_item(json_path, spot, name)

        return data[spot][name]
    except Exception as e:
        print(f"Something went wrong while reading json file: {e}")
        
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
    try:
        with open(json_file_path, 'r') as json_file:
            data = json.load(json_file)
            data[spot][key] = value

        with open(json_file_path, 'w') as json_file:
            json.dump(data, json_file, indent=4)
    except Exception as e:
        print(f"Something went wrong while modifying Json file: {e}")

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

    global reshade
    
    if not os.path.exists("Players\\Mp.cfg"):
        if not os.path.exists("Players"):
            os.mkdir("Players")
        
        try:
            path_to_cfg = f"{os.getcwd()}\\files\\Players\\Mp.cfg"
            shutil.copy(path_to_cfg, os.getcwd())
        except Exception as e:
            print(e)

    if not os.path.exists("LPC\\.manifest") or not os.path.exists("LPC\\core_ffotd_tu23_639_cf92ecf4a75d3f79.ff") or not os.path.exists("LPC\\core_playlists_tu23_639_cf92ecf4a75d3f79.ff"):
        if os.path.exists(f"{os.getcwd()}\\LPC"):
            try:
                shutil.rmtree(f"{os.getcwd()}\\LPC")
            except Exception as e:
                print(e)
        
        path_to_lpc = f"{os.getcwd()}\\files\\LPC"
        try:
            shutil.copytree(path_to_lpc, f"{os.getcwd()}\\LPC")
        except Exception as E:
            print(f"Error copying files: {E}")

    if which == "solo":
        try:

            if reshade == True:
                path_to_dll = f"{os.getcwd()}\\files\\reshade_solo\\powrprof.dll"
            else:
                path_to_dll = f"{os.getcwd()}\\files\\solo\\d3d11.dll"

            if os.path.exists(path_to_dll):
                shutil.copy(path_to_dll, os.getcwd()) 
        except Exception as e:
            print(f"Error while copying dll files: {e}")

    elif which == "multi":
        try:
            if reshade == True:
                path_to_dll = f"{os.getcwd()}\\files\\reshade_mp\\powrprof.dll"
            else:
                path_to_dll = f"{os.getcwd()}\\files\\mp\\d3d11.dll"

            if os.path.exists(path_to_dll):
                shutil.copy(path_to_dll, os.getcwd())
        except Exception as e:
            print(f"Error while copying dll files: {e}")

        ip_address = get_json_item('project-bo4.json', 'demonware', 'ipv4')
        if ip_address == "":
            click = set_ip()

            if click == 0:
                return

            if ip_address == "" and click == QDialog.Accepted:
                start_game("multi")
                return
            else:
                return

    try:
        window.hide()

        try:
            window.player.stop()
        except Exception as e:
            print(f"Error: {e}")

        try:
            process = subprocess.Popen(["BlackOps4.exe"])
            process.wait()
        except Exception as e:
            print(f"Error: {e}")

        sys.exit()
    except Exception as e:
        print(f"Error: {e}")

def checkbox_state_change(state):
    global reshade
    if state == 2:
        reshade = True
        print("Checkbox is checked")
    else:
        print("Checkbox is unchecked")
        reshade = False

class launcher(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()
        
    def initUI(self):

        layout = QVBoxLayout()
        self.setWindowTitle('Project-BO4')
        self.setWindowIcon(QIcon('files\\images\\exe_icon_bo4.ico'))
        title_layout = QVBoxLayout()
        try:
            
            images_folder = os.path.join(os.getcwd(), "files\\images")
            files = [f for f in os.listdir(images_folder) if f.lower().endswith(( '.gif', '.jpg', '.jpeg', '.jfif', '.pjpeg', '.pjp', '.png'))]
            random_background = random.choice(files)
        
            if random_background.lower().endswith(('.gif')):

                self.movie = QMovie(os.path.join(os.getcwd(), f"files\\images\\{random_background}"))
                background_label = QLabel(self)
                layout.addWidget(background_label)
                background_label.setMovie(self.movie)
                self.movie.start()

                self.setStyleSheet("background-color: #333;")

                buttons_layout = QHBoxLayout()
                checkbox = QCheckBox("Reshade")
                buttons_layout.addWidget(checkbox)
                checkbox.stateChanged.connect(checkbox_state_change)

            elif random_background.lower().endswith(( '.jpg', '.jpeg', '.jfif', '.pjpeg', '.pjp', '.png' )):

                background_label = QLabel(self)
                background_image = QPixmap(os.path.join(os.getcwd(), f"files\\images\\{random_background}"))
                background_label.setPixmap(background_image)
                background_label.setGeometry(0, 0, background_image.width(), background_image.height())
                self.setFixedSize(background_image.width(), background_image.height())

                self.title = QLabel("")
                title_layout.addWidget(self.title)
                layout.addLayout(title_layout)

                check_box = QVBoxLayout()
        
                checkbox = QCheckBox("Reshade")
                check_box.addWidget(checkbox)

                checkbox.stateChanged.connect(checkbox_state_change)
                layout.addLayout(check_box)
            
            #Add startup sound
            try:
                sound_folder = os.path.join(os.getcwd(), "files\\sounds")
                sound_files = [f for f in os.listdir(sound_folder) if f.lower().endswith('.mp3')]
                random_sound_file = random.choice(sound_files)

                sound_path = os.path.join(sound_folder, random_sound_file)

                self.player = QMediaPlayer()
                sound_url = QUrl.fromLocalFile(sound_path)
                content = QMediaContent(sound_url)
                self.player.setMedia(content)
                self.player.setVolume(30)
                self.player.play()
            
            except Exception as e:
                print(f"Something went wrong with start-up sound files: {e}")
            
        except Exception as e:
            print(f"Something went wrong with start up image, gif or sound files: {e}")

        
        if not random_background.lower().endswith(('.gif')):
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

    app.setStyle("Fusion")  # You can also use "Windows", "Macintosh", or "Fusion" for cross-platform style
    palette = app.palette()
    palette.setColor(QPalette.Window, Qt.black)  # Set the window background color
    palette.setColor(QPalette.WindowText, Qt.white)  # Set the window text color
    app.setPalette(palette)


    window = launcher()
    window.show()
    sys.exit(app.exec_())
