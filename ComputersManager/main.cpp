#include "computers_manager.h"
#include <QtWidgets/QApplication>
#include <Windows.h>
#include "login_window.h"
#include "server_connector.h"

int main(int argc, char *argv[])
{
    SetConsoleOutputCP(CP_UTF8);

    ServerInfo server_info;
    server_info.ip = "192.168.190.132";
    server_info.port = 22;
    server_info.user = "ymc";
    server_info.password = "11";
    ServerConnector * server_connector = new ServerConnector(server_info);
    /*auto ret = server_connector->ExecCommand("ifconfig");
    std::cout << ret << std::endl;*/
    try
    {
        //server_connector->DownloadFileFromServer2Local("/home/ymc/test.txt", "D:\\test.txt");
        server_connector->UploadFileFromLocal2Server("D:\\test.txt", "/home/ymc/test.txt");
    }
    catch (const std::exception& e)
    {
        BDEBUG(e.what());
    }
    
    delete server_connector;



    QApplication a(argc, argv);
    ComputersManager w;
    return a.exec();
}
