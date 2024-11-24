#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include "TcpServer.h"// Подключаем серверную часть
#include "TcpClient.h"// Подключаем клиентскую часть
#include "game.h"//// Подключаем саму игру
#include "Global_logger.h" // Подключаем глобальный логгер

std::unordered_map<std::string, std::string> readConfig(){ //чтение конфига
    std::unordered_map<std::string, std::string> config;//словарь для хранения параметров и значений
    std::ifstream file("config.ttt");//четкое название файла

    if (!file.is_open()){
        globalLogger.log("Error: no config file.");
        throw "Error: no config file";
    }

    std::string line;
    while (std::getline(file, line)){//читаем по строкам
        // Пропускаем пустые строки и строки, начинающиеся с '#'
        if (line.empty() || line[0] == '#') continue;//для комментариев

        // Находим позицию символа ':'
        size_t pos = line.find(':');
        if (pos == std::string::npos){//если не нашел разделителя параметров и их значений
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
    globalLogger.log("Configuration file read successfully.");
    return config;
}


int main(){
    globalLogger.log("Program started.");
    std::unordered_map<std::string, std::string> config;
    try{
        config = readConfig();
    }
    catch(const std::exception& e){
        globalLogger.log(std::string("Exception: ") + e.what());
        std::cout << e.what() << std::endl;
        return 2;
    }

    //Для данных игрока
    int my_row=-1;
    int my_col=-1;
    int my_len_win_line=-1;
    int my_step_time=-1;
    int port=0;
    std::string playername_1="";
    std::string my_password="";
    std::string type_person="";
    std::string ip="";
    //данные другого игрока
    std::string playername_2="";
    std::string other_password="";
    //сетевая часть
    TcpServer* server = nullptr;
    TcpClient* client = nullptr;

    bool first_step = false;//для хранения данных о том, кто ходит первым(0 - сервер, 1 - клиент)
    bool type_connection=false;//тип игрока в соединении 0 - сервер, 1 - клиент.
    bool connection_check=false;//для проверок соединения
    bool send_check=true;//проверка получения сообщений
    //распределение параметров по переменноым, заданным ранее
    for(auto pos=config.begin(); pos!=config.end(); ++pos){
        if((*pos).first=="username"){
            playername_1=(*pos).second;
        } else if((*pos).first=="type"){
            type_person=(*pos).second;
        } else if((*pos).first=="row_count"){
            my_row=std::atoi(((*pos).second).c_str());
        } else if((*pos).first=="col_count"){
            my_col=std::atoi(((*pos).second).c_str());
        } else if((*pos).first=="win_line_len"){
            my_len_win_line=std::atoi(((*pos).second).c_str());
        } else if((*pos).first=="step_time"){
            my_step_time=std::atoi(((*pos).second).c_str());
        } else if((*pos).first=="port"){
            port=std::atoi(((*pos).second).c_str());
        } else if((*pos).first=="ip"){
            ip=(*pos).second;
        } else {
            globalLogger.log(pos->first + " is not a valid game config.");
        }
    }
    //проверка на правильность конфигов
    if(my_row<=0 || my_col<=0 || my_len_win_line<=0 || my_step_time<=0 || playername_1==""){
        globalLogger.log("Configuration missing critical game parameters.");
        return 1;
    }
    //ввод пароля
    std::cout<<"Write password for game:";
    std::cin >> my_password;
    //сетевая часть
    if(type_person=="server"){// Логика сервера
        globalLogger.log("Starting as server...");
        //для параметров клиента
        int other_row=-1;
        int other_col=-1;
        int other_len_win_line=-1;
        int other_step_time=-1;
        //сетевая часть из TcpServer с проверками
        server = new TcpServer();
        if(!(server->check_start())){
            globalLogger.log("Server failed to initialize.");
            delete server;
            return -4;
        }
        connection_check=server->startServer(port);
        if(!connection_check){
            globalLogger.log("Failed to bind server to port " + std::to_string(port) + ".");
            delete server;
            return -2;
        }
        connection_check=server->acceptClient();
        if(!connection_check){
            globalLogger.log("Failed to accept client.");
            delete server;
            return -3;
        }
        globalLogger.log("Client connected successfully.");
        // Ожидание подключения клиента
        //Проверка пароля
        other_password = server->receiveMessage();
        if(my_password!=other_password){
            globalLogger.log("Password mismatch. Disconnecting client.");
            server->sendMessage("wr");
            if(!send_check){
                delete server;
                return -8;
            }
            delete server;
            return -5;
        } else if(other_password.empty()){
            delete server;
            return -8;
        } else {
            server->sendMessage("rr");
            if(!send_check){
                delete server;
                return -8;
            }
        }
        //Получение параметров поля и времени клиента для проверки
        std::string client_answer=server->receiveMessage();
        if(client_answer.empty()){
            delete server;
            return -8;
        }
        char div_char[3]={'_', '|', '^'};
        size_t first_div=0;
        size_t second_div=0;
        for(int i=0; i<3; ++i){
            second_div=first_div;
            first_div= client_answer.find(div_char[i]);
            switch (i){
            case 0:
                other_row=std::stoi(client_answer.substr(second_div, first_div));
                break;
            case 1:
                other_col=std::stoi(client_answer.substr(second_div, first_div));
                break;
            case 2:
                other_len_win_line=std::stoi(client_answer.substr(second_div, first_div));
                break;
            default:
                break;
            }
        }
        other_step_time=std::stoi(client_answer.substr(second_div, first_div));

        if(my_row!=other_row || my_col!=other_col || my_len_win_line!=other_len_win_line || my_step_time!=other_step_time){
            globalLogger.log("Main client's parametrs is wrong. Disconnecting client.");
            send_check=server->sendMessage("wr");
            if(!send_check){
                delete server;
                return -8;
            }
            delete server;
            return -6;
        } else {
            server->sendMessage("wr");
            if(!send_check){
                delete server;
                return -8;
            }
        }
        playername_2 = server->receiveMessage(); // Получаем имя клиента
        if(playername_2.empty()){
            delete server;
            return -8;
        }
        globalLogger.log("Read client name");
        send_check=server->sendMessage(playername_1);      // Отправляем свое имя
        if(!send_check){
            delete server;
            return -8;
        }
        globalLogger.log("Send our name");
        // Определение очередности хода
        srand(static_cast<unsigned>(time(0))); // Инициализация генератора случайных чисел
        first_step = (rand() % 2 == 0); // Случайное определение очередности
        send_check=server->sendMessage(first_step ? "1" : "0"); // Отправляем очередность
        if(!send_check){
            delete server;
            return -8;
        }
        globalLogger.log("Choose and send first_step");
        globalLogger.log("Server setup complete.");
    } else if(type_person=="client"){// Логика клиента
        globalLogger.log("Starting as client...");
        std::string server_answer="";
        type_connection=true;
        // Создание клиента
        client = new TcpClient();
        if(!(client->check_start())){
            globalLogger.log("Client failed to initialize.");
            delete client;
            return -4;
        }
        connection_check=client->connectToServer(ip, port);
        if(!connection_check){
            globalLogger.log("Failed to connect to server at " + ip + ":" + std::to_string(port) + ".");
            delete client;
            return -2;
        }

        send_check=client->sendMessage(my_password);
        if(!send_check){
            delete client;
            return -7;
        }
        server_answer = client->receiveMessage();
        if(client->receiveMessage()=="wr"){
            globalLogger.log("Password wrong. Disconect");
            delete client;
            return -5;
        } else if(server_answer.empty()){
            delete client;
            return -8;
        }
        //отправка параметров поля и времени на ход для сверки
        send_check=client->sendMessage(std::to_string(my_row)+"_"+std::to_string(my_col)+"|"+std::to_string(my_len_win_line)+"^"+std::to_string(my_step_time));
        if(!send_check){
            delete client;
            return -7;
        }
        server_answer = client->receiveMessage();
        if(client->receiveMessage()=="wr"){
            globalLogger.log("Main parametrs wrong. Disconect");
            delete client;
            return -6;
        } else if(server_answer.empty()){
            delete client;
            return -8;
        }
        // Отправляем свое имя серверу
        send_check=client->sendMessage(playername_1);
        if(!send_check){
            delete client;
            return -7;
        }
        globalLogger.log("Send client name");
        playername_2 = client->receiveMessage(); // Получаем имя сервера
        if(playername_2.empty()){
            delete client;
            return -8;
        }
        globalLogger.log("Read server name");

        // Получение очередности хода
        std::string first_step_answer=client->receiveMessage();
        if(first_step_answer.empty()){
            delete client;
            return -8;
        }
        first_step = (first_step_answer == "1");
        globalLogger.log("write first_step");
        globalLogger.log("Client setup complete.");
    } else {
        std::cout<< "Wrong player type." << std::endl;
        return 2;
    }
    globalLogger.log("Starting Tic-Tac-Toe game...");
    Tic_tac_toe mainest(my_row, my_col, my_len_win_line, my_step_time, playername_1, playername_2, first_step, server, client, type_connection);
    mainest.game();
    globalLogger.log("Game ended.");
    delete server;
    delete client;
    globalLogger.log("Program ended.");
    return 0;
}