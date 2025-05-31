#pragma once
#include "log_common.h"
#include "log_backend.h"
#include <string>

class Logger
{
  public:
    Logger(std::string tag, LogBackend* backend);

    void log(LogLevel level, std::string content);
    void debug(std::string content);
    void info(std::string content);
    void warn(std::string content);
    void error(std::string content);
    void critical(std::string content);

  private:
    std::string m_tag;
    LogBackend* m_backend;
};