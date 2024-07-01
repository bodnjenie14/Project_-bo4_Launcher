# Project_-bo4_Launcher 

![2](https://github.com/bodnjenie14/Project_-bo4_Launcher/assets/126781031/267ffdd6-9146-4041-96b0-34ffdedc03d8)


Launcher for Shield - Project Black ops 4 -  https://github.com/project-bo4/shield-development

What this does:

Easy way of launching the client.

Our Discord Server : https://discord.gg/AXECAzJJGU

# Compile instructions 

To compile this project, it is necessary to install Python3. Python3 can be downloaded from python.org

1. Install dependencies
This project depends on Pyinstaller and PyQT5
`pip install pyinstaller pyqt5`

2. Compiling the project into a .exe file using Pyinstaller
> It may be necessary to disable Windows Defender while compiling, otherwise it may produce an unusable executable

`pyinstaller --onefile --windowed --icon="files\images\exe_icon_bo4.ico" --noconfirm project-bo4.py`

Coded by Unknown Love
