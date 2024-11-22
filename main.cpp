#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include "TcpServer.h"
#include "TcpClient.h"
#include "game.h"

std::unordered_map<std::string, std::string> readConfig() {
    std::unordered_map<std::string, std::string> config;
    std::ifstream file("config.ttt");

    if (!file.is_open()) {
        throw "Error: no config file";
    }

    std::string line;
    while (std::getline(file, line)) {
        // Пропускаем пустые строки и строки, начинающиеся с '#'
        if (line.empty() || line[0] == '#') continue;

        // Находим позицию символа ':'
        size_t pos = line.find(':');
        if (pos == std::string::npos){
            continue;
        }

        // Извлекаем ключ и значение
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Удаляем лишние пробелы (если необходимо)
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // Сохраняем в словарь
        config[key] = value;
    }

    file.close();
    return config;
}


int main(){
    std::unordered_map<std::string, std::string> config;
    try{
        config = readConfig();
    }
    catch(const std::exception& e){
        std::cout << e.what() << std::endl;
    }
    
    int row=-1;
    int col=-1;
    int len_win_line=-1;
    int step_time=-1;
    int port=0;
    std::string playername_1="";
    std::string playername_2="";
    std::string my_password="";
    std::string other_password="";
    std::string type_person="";
    std::string ip="";
    TcpServer* server = nullptr;
    TcpClient* client = nullptr;
    bool first_step = false;
    bool type_connection=false;
    bool connection_check=false;
    for(auto pos=config.begin(); pos!=config.end(); ++pos){
        if((*pos).first=="username"){
            playername_1=(*pos).second;
        } else if((*pos).first=="password"){
            my_password=(*pos).second;
        } else if((*pos).first=="type"){
            type_person=(*pos).second;
        } else if((*pos).first=="row_count"){
            row=std::atoi(((*pos).second).c_str());
        } else if((*pos).first=="col_count"){
            col=std::atoi(((*pos).second).c_str());
        } else if((*pos).first=="win_line_len"){
            len_win_line=std::atoi(((*pos).second).c_str());
        } else if((*pos).first=="step_time"){
            step_time=std::atoi(((*pos).second).c_str());
        } else if((*pos).first=="port"){
            port=std::atoi(((*pos).second).c_str());
        } else if((*pos).first=="ip"){
            ip=(*pos).second;
        } else {
            std::cout<<(*pos).first << " is not config for game" << std::endl;
        }
    }
    if(row<=0 || col<=0 || len_win_line<=0 || step_time<=0 || playername_1==""){
        std::cout << "Your configs don't have important for game" << std::endl;
        return 1;
    }
    if(type_person=="server"){
        server = new TcpServer();
        if(!(server->check_start())){
            delete server;
            return -4;
        }
        connection_check=server->startServer(port);
        if(!connection_check){
            delete server;
            return -2;
        }
        connection_check=server->acceptClient();
        if(!connection_check){
            delete server;
            return -3;
        }
        // Ожидание подключения клиента
        std::cout << "Waiting for a client to connect." << std::endl;
        other_password = server->receiveMessage();
        if(my_password!=other_password){
            server->sendMessage("wr");
            delete server;
            return -5;
        }
        playername_2 = server->receiveMessage(); // Получаем имя клиента
        server->sendMessage(playername_1);      // Отправляем свое имя

        // Определение очередности хода
        srand(static_cast<unsigned>(time(0))); // Инициализация генератора случайных чисел
        first_step = (rand() % 2 == 0); // Случайное определение очередности
        server->sendMessage(first_step ? "1" : "0"); // Отправляем очередность
    } else if(type_person=="player"){
        type_connection=true;
        // Создание клиента
        client = new TcpClient();
        if(!(client->check_start())){
            delete client;
            return -4;
        }
        connection_check=client->connectToServer(ip, port);
        if(!connection_check){
            delete client;
            return -2;
        }

        // Отправляем свое имя серверу
        if(client->receiveMessage()=="wr"){
            delete client;
            return -5;
        }
        client->sendMessage(playername_1);
        playername_2 = client->receiveMessage(); // Получаем имя сервера

        // Получение очередности хода
        first_step = (client->receiveMessage() == "1");
    } else {
        std::cout<< "Wrong player type." << std::endl;
        return 2;
    }
    Tic_tac_toe mainest(row, col, len_win_line, step_time, playername_1, playername_2, first_step, server, client, type_connection);
    mainest.game();
    if(!type_connection){
        delete server;
    } else {
        delete client;
    }
    return 0;
}