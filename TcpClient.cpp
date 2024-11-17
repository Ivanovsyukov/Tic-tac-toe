#include "TcpClient.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include<winsock2.h>
#include <WS2tcpip.h>
#include <tchar.h>

TcpClient::TcpClient(const std::string& ip, int port) : client_fd(-1), server_ip(ip), server_port(port) {}

TcpClient::~TcpClient() {
    if (client_fd != -1) {
        close(client_fd);
    }
}

void TcpClient::connectToServer() {
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Socket creation failed");
        throw std::runtime_error("Socket creation failed");
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        throw std::runtime_error("Invalid address");
    }

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        throw std::runtime_error("Connection failed");
    }

    std::cout << "Connected to server at " << server_ip << ":" << server_port << std::endl;
}

std::string TcpClient::receiveData() {
    char buffer[1024];
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        perror("Receive failed");
        throw std::runtime_error("Receive failed");
    }

    buffer[bytes_received] = '\0'; // Null-terminate the received string
    return std::string(buffer);
}

void TcpClient::sendData(const std::string& data) {
    ssize_t bytes_sent = send(client_fd, data.c_str(), data.size(), 0);
    if (bytes_sent == -1) {
        perror("Send failed");
        throw std::runtime_error("Send failed");
    }
}