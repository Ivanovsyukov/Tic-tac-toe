#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>

class TcpClient {
private:
    int client_fd;  // Файловый дескриптор клиента
    std::string server_ip;
    int server_port;

public:
    TcpClient(const std::string& ip, int port);
    ~TcpClient();

    void connectToServer();        // Подключение к серверу
    std::string receiveData();     // Получение данных от сервера
    void sendData(const std::string& data); // Отправка данных серверу
};

#endif