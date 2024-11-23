#include "Logger.h"

Logger::Logger(const std::string &logPrefix) {
    // Создаем имя файла
    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::ostringstream fileName;
    fileName << logPrefix << "_"
             << 1900 + ltm->tm_year << "_"
             << 1 + ltm->tm_mon << "_"
             << ltm->tm_mday << "_"
             << ltm->tm_hour << "_"
             << ltm->tm_min << "_"
             << ltm->tm_sec << ".log";

    // Открываем файл
    logFile.open(fileName.str(), std::ios::out | std::ios::app);
    if (logFile.is_open()) {
        logFile << "Log started.\n";
    } else {
        std::cerr << "Failed to open log file." << std::endl;
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile << "Log closed.\n";
        logFile.close();
    }
}

void Logger::log(const std::string &message) {
    if (!logFile.is_open()) return;

    logFile << "[" << getTimeStamp() << "] " << message << "\n";
    logFile.flush();
}

std::string Logger::getTimeStamp() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
             1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
             ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return std::string(buffer);
}