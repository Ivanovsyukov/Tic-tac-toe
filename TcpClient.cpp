#include "TcpClient.h"
#include <iostream>

TcpClient::TcpClient() {
    // Инициализация WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WinSock initialization failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Создаем сокет
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket!" << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

TcpClient::~TcpClient() {
    closesocket(clientSocket); // Закрываем сокет
    WSACleanup(); // Очищаем ресурсы WinSock
}

bool TcpClient::connectToServer(const std::string& ipAddress, int port) {
    // Настройка адреса сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());

    // Подключение к серверу
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection to server failed!" << std::endl;
        return false;
    }

    std::cout << "Connected to server: " << ipAddress << ":" << port << std::endl;
    return true;
}

bool TcpClient::sendMessage(const std::string& message) {
    if (send(clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send message!" << std::endl;
        return false;
    }
    return true;
}

std::string TcpClient::receiveMessage() {
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) {
        return ""; // Соединение закрыто или ошибка
    }
    return std::string(buffer, bytesReceived);
}