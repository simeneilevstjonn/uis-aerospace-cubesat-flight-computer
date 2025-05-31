#include "logger.h"
#include <ctime>

Logger::Logger(std::string tag, LogBackend* backend)
{
    m_tag = tag;
    m_backend = backend;
}

void Logger::log(LogLevel level, std::string content)
{
    LogEntry entry;
    entry.level = level;
    time(&entry.time);
    entry.tag = m_tag;
    entry.content = content;

    m_backend->push_entry(entry);
}

void Logger::debug(std::string content)
{
    log(LogLevel::Debug, content);
}

void Logger::info(std::string content)
{
    log(LogLevel::Info, content);
}

void Logger::warn(std::string content)
{
    log(LogLevel::Warn, content);
}

void Logger::error(std::string content)
{
    log(LogLevel::Error, content);
}

void Logger::critical(std::string content)
{
    log(LogLevel::Critical, content);
}

void Logger::debug(std::string content);