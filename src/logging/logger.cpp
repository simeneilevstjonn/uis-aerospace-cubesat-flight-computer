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