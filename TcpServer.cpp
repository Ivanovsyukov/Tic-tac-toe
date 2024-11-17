#include "TcpServer.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include<winsock2.h>

TcpServer::TcpServer(int port) : server_fd(-1), client_fd(-1), port(port), is_running(false) {}

TcpServer::~TcpServer() {
    stop();
}

void TcpServer::start() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        throw std::runtime_error("Socket creation failed");
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        throw std::runtime_error("Bind failed");
    }

    if (listen(server_fd, 1) == -1) {
        perror("Listen failed");
        throw std::runtime_error("Listen failed");
    }

    std::cout << "Server is listening on port " << port << std::endl;

    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("Accept failed");
        throw std::runtime_error("Accept failed");
    }

    std::cout << "Client connected: " << inet_ntoa(client_addr.sin_addr) << std::endl;
    is_running = true;
}

void TcpServer::stop() {
    if (client_fd != -1) {
        close(client_fd);
        client_fd = -1;
    }
    if (server_fd != -1) {
        close(server_fd);
        server_fd = -1;
    }
    is_running = false;
}

std::string TcpServer::receiveData() {
    if (!is_running) throw std::runtime_error("Server is not running");

    char buffer[1024];
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        perror("Receive failed");
        throw std::runtime_error("Receive failed");
    }

    buffer[bytes_received] = '\0'; // Null-terminate the received string
    return std::string(buffer);
}

void TcpServer::sendData(const std::string& data) {
    if (!is_running) throw std::runtime_error("Server is not running");

    ssize_t bytes_sent = send(client_fd, data.c_str(), data.size(), 0);
    if (bytes_sent == -1) {
        perror("Send failed");
        throw std::runtime_error("Send failed");
    }
}