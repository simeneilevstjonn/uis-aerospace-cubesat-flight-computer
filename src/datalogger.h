#pragma once
#include <queue>
#include "logger.h"
#include <string>
#include <cstdio>
#include "accelerometer.h"
#include "barometer.h"
#include <thread>
#include <mutex>

class DataLogger
{
  public:
    DataLogger(Logger* logger, std::string base_directory);

    void queue_acceleration_sample(AccelerometerSample& sample);
    void queue_barometer_sample(BarometerSample& sample);
    void queue_gnss_sample(std::string& sample);

  private:
    Logger* m_logger;

    std::queue<AccelerometerSample> m_accelerometer_queue;
    std::queue<BarometerSample> m_barometer_queue;
    std::queue<std::string> m_gnss_queue;

    FILE* m_accelerometer_file;
    FILE* m_barometer_file;
    FILE* m_gnss_file;

    std::mutex m_accelerometer_mutex;
    std::mutex m_barometer_mutex;
    std::mutex m_gnss_mutex;

    std::thread m_thread;

    void thread_entry();
    static void static_thread_entry(DataLogger* dataLogger);
};