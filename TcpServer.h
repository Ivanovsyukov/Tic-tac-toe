#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib") // Линковка библиотеки WinSock

class TcpServer{
private:
    SOCKET serverSocket; // Серверный сокет
    SOCKET clientSocket; // Сокет клиента
    sockaddr_in serverAddr; // Адрес сервера
    sockaddr_in clientAddr; // Адрес клиента

public:
    TcpServer();
    ~TcpServer();

    bool startServer(int port);
    bool acceptClient();
    bool sendMessage(const std::string& message);
    std::string receiveMessage();
    bool check_start();
};

#endif