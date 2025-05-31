#pragma once
#include <ctime>
#include <cstdint>
#include <string>

enum class LogLevel : uint8_t
{
    Debug = 0,
    Info = 1,
    Warn = 2,
    Error = 3,
    Critical = 4,
    NoLog = 255
};

struct LogEntry
{
    LogLevel level;
    time_t time;
    std::string tag;
    std::string content;
};