#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>

class TcpServer {
private:
    int server_fd; // Файловый дескриптор сервера
    int client_fd; // Файловый дескриптор подключенного клиента
    int port;      // Порт для прослушивания
    bool is_running;

public:
    TcpServer(int port);
    ~TcpServer();

    void start();                 // Запуск сервера
    void stop();                  // Остановка сервера
    std::string receiveData();    // Получение данных от клиента
    void sendData(const std::string& data); // Отправка данных клиенту
};

#endif