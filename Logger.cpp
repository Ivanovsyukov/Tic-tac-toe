#include "Logger.h"

Logger::Logger(const std::string &logPrefix) {
    // Создаем имя файла
    time_t now = time(0);//время в формате UNIX
    tm *ltm = localtime(&now);//переводим в более удобный формат
    std::ostringstream fileName;//создаем структуру для сборки строки
    fileName << logPrefix << "_"
             << 1900 + ltm->tm_year << "_"
             << 1 + ltm->tm_mon << "_"
             << ltm->tm_mday << "_"
             << ltm->tm_hour << "_"
             << ltm->tm_min << "_"
             << ltm->tm_sec << ".log";//собираем название файла

    // Открываем файл
    logFile.open(fileName.str(), std::ios::out | std::ios::app);
    if (logFile.is_open()) {
        logFile << "Log started.\n";
    } else {
        std::cerr << "Error: Unable to open log file. Logging will be disabled." << std::endl;
    }
}

Logger::~Logger(){
    if (logFile.is_open()) {
        logFile << "[" << getTimeStamp() << "] " << "Log closed.\n";
        logFile.close();
    }
}

void Logger::log(const std::string &message){
    if (!logFile.is_open()) return;

    logFile << "[" << getTimeStamp() << "] " << message << "\n";//создаем сообщение
    logFile.flush();//принудительная запись на диск
}

std::string Logger::getTimeStamp(){
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[20];//для быстроты мы сразу используем константное поле для даты
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
             1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
             ltm->tm_hour, ltm->tm_min, ltm->tm_sec);//заполняем дату по шаблону год месяц день час минута секунда.
    return std::string(buffer); //Превращаем в строку
}