#include "logger.h"

Logger& Logger::Instance() {
    static Logger instance;
    return instance;
}

void Logger::setLogLevel(LogLevel level) { m_logLevel = level; }

int Logger::getColorCode(LogLevel level) {
    switch (level) {
        case LogLevel::OK:
            return 34;
        case LogLevel::Debug:
            return 36;
        case LogLevel::Info:
            return 32;
        case LogLevel::Warning:
            return 33;
        case LogLevel::Error:
            return 31;
        default:
            return 0;
    }
}

void Logger::log(const std::string& message, LogLevel level) {
    if (level <= m_logLevel) {
        std::cout << "\033[1;" << getColorCode(level) << "m";

        switch (level) {
            case LogLevel::OK:
                std::cout << "[OK] ";
                break;
            case LogLevel::Debug:
                std::cout << "[DEBUG] ";
                break;
            case LogLevel::Info:
                std::cout << "[INFO] ";
                break;
            case LogLevel::Warning:
                std::cout << "[WARNING] ";
                break;
            case LogLevel::Error:
                std::cout << "[ERROR] ";
                break;
        }

        std::cout << message << "\033[0m\n";
    }
}

Logger::Logger() { m_logLevel = LogLevel::Info; }