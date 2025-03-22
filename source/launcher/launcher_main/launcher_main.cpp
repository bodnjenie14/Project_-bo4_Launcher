#include "std_include.hpp"
#include "launcher_main_window.hpp"

#ifdef _WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    int argc = __argc;
    char** argv = __argv;
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
#else
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
#endif
