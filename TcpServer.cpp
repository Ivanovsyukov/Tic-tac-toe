#include "TcpServer.h"
#include <iostream>

TcpServer::TcpServer() {
    // Инициализация WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WinSock initialization failed!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Создаем серверный сокет
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket!" << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
}

TcpServer::~TcpServer() {
    closesocket(clientSocket); // Закрываем клиентский сокет
    closesocket(serverSocket); // Закрываем серверный сокет
    WSACleanup(); // Очищаем ресурсы WinSock
}

bool TcpServer::startServer(int port) {
    // Настройка адреса сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Привязка сокета
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket!" << std::endl;
        return false;
    }

    // Переводим сокет в режим прослушивания
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on socket!" << std::endl;
        return false;
    }

    std::cout << "Server started on port " << port << std::endl;
    return true;
}

bool TcpServer::acceptClient() {
    int clientAddrSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to accept client!" << std::endl;
        return false;
    }

    std::cout << "Client connected!" << std::endl;
    return true;
}

bool TcpServer::sendMessage(const std::string& message) {
    if (send(clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send message!" << std::endl;
        return false;
    }
    return true;
}

std::string TcpServer::receiveMessage() {
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) {
        return ""; // Соединение закрыто или ошибка
    }
    return std::string(buffer, bytesReceived);
}