#pragma once
#include "log_common.h"
#include "log_backend.h"
#include <string>
#include <cstring>

class Logger
{
  private:
    std::string m_tag;
    LogBackend* m_backend;
    static constexpr size_t LOG_CONTENT_BUFFER_SIZE = 2048;

  public:
    Logger(std::string tag, LogBackend* backend);

    void log(LogLevel level, std::string content);

    template <typename... Args> void log(LogLevel level, const char* format, Args&&... args)
    {
        char* log_content = new char[LOG_CONTENT_BUFFER_SIZE];

        snprintf(log_content, LOG_CONTENT_BUFFER_SIZE, format, args...);

        log_content[LOG_CONTENT_BUFFER_SIZE - 1] = 0;

        auto logstr = std::string(log_content);
        delete[] log_content;

        log(level, logstr);
    }

    template <typename... Args> inline void debug(const char* format, Args&&... args)
    {
        log(LogLevel::Debug, format, args...);
    }

    template <typename... Args> inline void info(const char* format, Args&&... args)
    {
        log(LogLevel::Info, format, args...);
    }

    template <typename... Args> inline void warn(const char* format, Args&&... args)
    {
        log(LogLevel::Warn, format, args...);
    }

    template <typename... Args> inline void error(const char* format, Args&&... args)
    {
        log(LogLevel::Error, format, args...);
    }

    template <typename... Args> inline void critical(const char* format, Args&&... args)
    {
        log(LogLevel::Critical, format, args...);
    }
};