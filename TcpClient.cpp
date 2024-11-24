#include "TcpClient.h"
#include "Global_logger.h"

TcpClient::TcpClient():isInitialized(false){
    // Инициализация WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        globalLogger.log("WinSock initialization failed!");
        exit(EXIT_FAILURE);
    }

    // Создаем сокет
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET){
        globalLogger.log("Failed to create socket!");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Если всё прошло успешно, устанавливаем isInitialized в true
    globalLogger.log("Client socket created successfully.");
    isInitialized = true;
}

TcpClient::~TcpClient(){
    closesocket(clientSocket); // Закрываем сокет
    WSACleanup(); // Очищаем ресурсы WinSock
    globalLogger.log("Client resources cleaned up.");
}

bool TcpClient::connectToServer(const std::string& ipAddress, int port){
    if (!isInitialized) return false;
    // Настройка адреса сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());

    // Подключение к серверу
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
        globalLogger.log("Failed to connect to server " + ipAddress + ":" + std::to_string(port));
        return false;
    }

    globalLogger.log("Connected to server " + ipAddress + ":" + std::to_string(port));
    return true;
}

bool TcpClient::sendMessage(const std::string& message){
    if (!isInitialized) return false;
    //отправление сообщения
    if (send(clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
        globalLogger.log("Failed to send message!:"+std::string(strerror(errno)));
        return false;
    }
    globalLogger.log("Good send message!");
    return true;
}

std::string TcpClient::receiveMessage(){
    if (!isInitialized) return "";
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);//запись в buffer, а длину сообщения в bytesReceived
    if (bytesReceived <= 0){
        globalLogger.log("Bad read message!:"+std::string(strerror(errno)));
        return ""; // Соединение закрыто или ошибка
    }
    globalLogger.log("Good read message!");
    return std::string(buffer, bytesReceived);//выводим строку из буфера по длине полученного сообщения
}

bool TcpClient::check_start(){
    return isInitialized;
}