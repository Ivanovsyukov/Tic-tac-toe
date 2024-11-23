#include <string>
#include <winsock2.h>
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#pragma comment(lib, "ws2_32.lib") // Линковка библиотеки WinSock

class TcpClient{
private:
    SOCKET clientSocket; // Сокет клиента
    sockaddr_in serverAddr; // Адрес сервера

    bool isInitialized; //Инициализирован ли сокет
public:
    TcpClient();
    ~TcpClient();

    bool connectToServer(const std::string& ipAddress, int port);
    bool sendMessage(const std::string& message);
    std::string receiveMessage();
    bool check_start();
};

#endif