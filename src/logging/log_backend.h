#pragma once
#include "log_common.h"
#include <queue>
#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>

class LogBackend
{
  public:
    void register_stdout(LogLevel level);
    void register_file(const char* directory, const char* basename, LogLevel level);

    void push_entry(LogEntry entry);

    void enable();

  private:
    void thread_entry();
    std::string log_level_to_string(LogLevel level);

    std::thread m_thread;

    LogLevel m_stdout_level = LogLevel::NoLog;
    LogLevel m_file_level = LogLevel::NoLog;
    std::ofstream m_out_file;

    std::queue<LogEntry> m_queue = std::queue<LogEntry>();
    std::mutex m_queue_mutex;
    std::condition_variable m_data_ready_cv;
    bool m_data_ready = false;

    friend void static_thread_entry(LogBackend* backend);
};