# The Simpsons Tapped Out Ipa and Apk Patcher.

A C++ Qt-based GUI application for patching The Simpsons Tapped Out APK/IPA. This application allows you to modify the apk to point to my private server.

My private server for Tapped Out can be found here : [TSTO Private Server](https://github.com/bodnjenie14/Tsto---Simpsons-Tapped-Out---Private-Server/tree/main)

Very Simple to use.

![image](https://github.com/user-attachments/assets/4d6cba6c-ca69-4e7b-88cc-cb82369696ae)

## Usage

1. Launch the application
2. Click "Check Dependencies" to ensure all required tools are installed
3. Click "Browse" to select an APK/IPA file
4. Enter the new Game Server URL and DLC Server URL
5. Click "Patch APK" to process the file

IP Address Example
Server IP: http://192.168.1.1:80
DLC IP: http://192.168.1.2:80

---

<h1 align="center">For the nerds</h1>

## Prerequisites

- Premake5
- OpenJDK 11 or later
- Visual Studio 2019 or later (for Windows)

## Building

1. Generate the project files using Premake:
```cmd
premake5 vs2019
```

3. Open the generated solution file in Visual Studio and build the project.


## Features

- APK decompilation and recompilation using apktool
- URL replacement in text-based files (.smali, .xml, .txt)
- Binary patching of .so files
- Dependency checking and installation
- User-friendly GUI interface

## Notes

- The application requires Java for APK tool operations
- Make sure you have write permissions in the application directory

