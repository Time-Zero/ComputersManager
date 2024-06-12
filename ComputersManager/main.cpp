#include "computers_manager.h"
#include <QtWidgets/QApplication>
#include <Windows.h>
#include "sql_service.h"
#include "login_window.h"
#include "libssh2.h"

int main(int argc, char *argv[])
{
    SetConsoleOutputCP(CP_UTF8);

    QApplication a(argc, argv);
    ComputersManager w;
    return a.exec();
}
