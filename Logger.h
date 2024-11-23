#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>
#ifndef LOGGER_H
#define LOGGER_H

class Logger {
public:
    Logger(const std::string &logPrefix); // Конструктор
    ~Logger(); // Деструктор

    void log(const std::string &message); // Логирование

private:
    std::ofstream logFile; // Переменная для лог-файла
    std::string getTimeStamp(); // Вспомогательный метод для создания временной метки
};

#endif // LOGGER_H