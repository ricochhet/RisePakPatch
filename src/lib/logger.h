#ifndef _RCHT_LOGGER_INCLUDED_
#define _RCHT_LOGGER_INCLUDED_

#pragma once

#include <iostream>
#include <string>
namespace RisePakPatch {
    enum class LogLevel { OK, Debug, Info, Warning, Error };

    class Logger {
    public:
        static Logger& Instance();
        void           setLogLevel(LogLevel level);
        int            getColorCode(LogLevel level);
        void           log(const std::string& message, LogLevel level = LogLevel::Info);

    private:
        Logger();
        Logger(const Logger&)            = delete;
        Logger& operator=(const Logger&) = delete;

        LogLevel m_logLevel;
    };
}

#endif  // _RCHT_LOGGER_INCLUDED_