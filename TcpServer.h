#include <winsock2.h>
#include <string>

#ifndef TCPSERVER_H
#define TCPSERVER_H

#pragma comment(lib, "ws2_32.lib") // Линковка библиотеки WinSock

class TcpServer{
private:
    SOCKET serverSocket; // Серверный сокет
    SOCKET clientSocket; // Сокет клиента
    sockaddr_in serverAddr; // Адрес сервера
    sockaddr_in clientAddr; // Адрес клиента
    
    bool isInitialized; //Инициализирован ли сокет
public:
    TcpServer();
    ~TcpServer();

    bool startServer(int port);
    bool acceptClient();
    bool sendMessage(const std::string& message);
    std::string receiveMessage();
    bool check_start() const;
};

#endif