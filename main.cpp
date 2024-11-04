#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include "game.h"

std::unordered_map<std::string, std::string> readConfig() {
    std::unordered_map<std::string, std::string> config;
    std::ifstream file("config.ttt");

    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл конфигурации."<<std::endl;
        return config;
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
    std::unordered_map<std::string, std::string> config = readConfig();
    int row=-1;
    int col=-1;
    int len_win_line=-1;
    int step_time=-1;
    std::string playername_1="";
    std::string playername_2="";
    for(auto pos=config.begin(); pos!=config.end(); ++pos){
        if((*pos).first=="username"){
            playername_1=(*pos).second;
        }else if((*pos).first=="password"){

        }else if((*pos).first=="row_count"){
            row=std::atoi(((*pos).second).c_str());
        }else if((*pos).first=="col_count"){
            col=std::atoi(((*pos).second).c_str());
        }else if((*pos).first=="win_line_len"){
            len_win_line=std::atoi(((*pos).second).c_str());
        }else if((*pos).first=="step_time"){
            step_time=std::atoi(((*pos).second).c_str());
        } else {
            std::cout<<(*pos).first << " - нет нужного конфига." << std::endl;
        }
    }
    if(row<=0 || col<=0 || len_win_line<=0 || step_time<=0 || playername_1==""){
        std::cout << "У тебя есть конфиг или конфиги с ошибкой" << std::endl;
        return 1;
    }
    Tic_tac_toe mainest(row, col, len_win_line, step_time, playername_1, playername_2);
    mainest.game();
    return 0;
}