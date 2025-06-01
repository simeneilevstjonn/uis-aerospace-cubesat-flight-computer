#include "log_backend.h"
#include <ctime>
#include <array>

void static_thread_entry(LogBackend* backend)
{
    backend->thread_entry();
}

LogBackend::LogBackend()
{
}

void LogBackend::register_stdout(LogLevel level)
{
    m_stdout_level = level;
}

void LogBackend::register_file(const char* directory, const char* basename, LogLevel level)
{
    time_t timestamp;
    time(&timestamp);

    tm datetime = *localtime(&timestamp);

    std::array<char, 1024> full_path;
    full_path[full_path.size() - 1] = 0;
    snprintf(full_path.data(), full_path.size(), "%s/%s-%04d-%02d-%02dT%02d:%02d:%02d", directory, basename, datetime.tm_year + 1900, datetime.tm_mon + 1, datetime.tm_mday,
             datetime.tm_hour, datetime.tm_min, datetime.tm_sec);

    // Overflow
    if (full_path[full_path.size() - 1] != 0)
    {
        throw -1;
    }

    m_out_file.open(full_path.data());
    m_file_level = level;
}

void LogBackend::push_entry(LogEntry& entry)
{
    std::lock_guard<std::mutex> lk(m_queue_mutex);
    m_queue.push(entry);

    m_data_ready = true;
    m_data_ready_cv.notify_all();
}

void LogBackend::enable()
{
    m_thread = std::thread(static_thread_entry, this);
}

void LogBackend::thread_entry()
{
    while (true)
    {
        // Await wakeup.
        std::mutex mtx;
        std::unique_lock<std::mutex> lck(mtx);
        while (!m_data_ready)
        {
            m_data_ready_cv.wait(lck);
        }

        bool file_needs_flush = false;
        bool stdout_needs_flush = false;

        while (!m_queue.empty())
        {
            auto element = m_queue.front();
            {
                std::lock_guard<std::mutex> lk(m_queue_mutex);
                m_queue.pop();
            }

            bool skip = element.level < m_file_level && element.level < m_stdout_level;
            if (skip)
            {
                continue;
            }

            std::array<char, 4096> encoded;
            tm datetime = *localtime(&element.time);

            snprintf(encoded.data(), encoded.size(), "[%s] %04d-%02d-%02d %02d:%02d:%02d [%s]: %s\n", log_level_to_string(element.level).c_str(), datetime.tm_year + 1900,
                     datetime.tm_mon + 1, datetime.tm_mday, datetime.tm_hour, datetime.tm_min, datetime.tm_sec, element.tag.c_str(), element.content.c_str());

            encoded[encoded.size() - 1] = 0;

            if (element.level >= m_file_level && m_out_file)
            {
                m_out_file << encoded.data();
                file_needs_flush = true;
            }

            if (element.level >= m_stdout_level)
            {
                std::cout << encoded.data();
                stdout_needs_flush = true;
            }
        }

        if (file_needs_flush)
        {
            m_out_file.flush();
        }
        if (stdout_needs_flush)
        {
            std::cout.flush();
        }
    }
}

std::string LogBackend::log_level_to_string(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warn:
        return "WARN";
    case LogLevel::Critical:
        return "CRITICAL";
    default:
        throw -1;
    }
}