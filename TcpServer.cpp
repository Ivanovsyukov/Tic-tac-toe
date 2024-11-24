#include "TcpServer.h"
#include "Global_logger.h"

TcpServer::TcpServer(): serverSocket(INVALID_SOCKET), clientSocket(INVALID_SOCKET), isInitialized(false){
    // Инициализация WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        globalLogger.log("WinSock initialization failed!");
        exit(EXIT_FAILURE);
    }

    // Создаем серверный сокет
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET){
        globalLogger.log("Failed to create socket!");
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    // Если всё прошло успешно, устанавливаем isInitialized в true
    globalLogger.log("Server socket created successfully.");
    isInitialized = true;
}

TcpServer::~TcpServer(){
    closesocket(clientSocket); // Закрываем клиентский сокет
    closesocket(serverSocket); // Закрываем серверный сокет
    WSACleanup(); // Очищаем ресурсы WinSock
    globalLogger.log("Server resources cleaned up.");
}

bool TcpServer::startServer(int port){
    if (!isInitialized) return false;
    // Настройка адреса сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Привязка сокета
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR){
        globalLogger.log("Failed to bind socket!");
        return false;
    }

    // Переводим сокет в режим прослушивания
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR){
        globalLogger.log("Failed to listen on socket!");
        return false;
    }

    globalLogger.log("Server started on port " + std::to_string(port));
    return true;
}

bool TcpServer::acceptClient(){
    if (!isInitialized) return false;
    int clientAddrSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET){
        globalLogger.log("Failed to accept client!");
        return false;
    }
    globalLogger.log("Client connected!");
    return true;
}

bool TcpServer::sendMessage(const std::string& message) {
    if (!isInitialized) return false;
    if (send(clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR){
        globalLogger.log("Failed to send message!");
        return false;
    }
    globalLogger.log("Good send message!");
    return true;
}

std::string TcpServer::receiveMessage(){
    if (!isInitialized) return "";
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0){
        globalLogger.log("Bad read message!");
        return ""; // Соединение закрыто или ошибка
    }
    globalLogger.log("Good read message!");
    return std::string(buffer, bytesReceived);
}

bool TcpServer::check_start() const{
    return isInitialized;
}