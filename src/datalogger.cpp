#include "datalogger.h"
#include <filesystem>
#include <ctime>
#include <cstddef>
#include <unistd.h>
#include <array>

DataLogger::DataLogger(Logger* logger, std::string base_directory)
{
    m_logger = logger;

    time_t timestamp;
    time(&timestamp);

    tm datetime = *localtime(&timestamp);

    std::array<char, 1024> episode_path;
    episode_path[episode_path.size() - 1] = 0;
    snprintf(episode_path.data(), episode_path.size(), "%s/%04d-%02d-%02dT%02d:%02d:%02d", base_directory.c_str(), datetime.tm_year + 1900, datetime.tm_mon + 1, datetime.tm_mday,
             datetime.tm_hour, datetime.tm_min, datetime.tm_sec);

    // Overflow
    if (episode_path[episode_path.size() - 1] != 0)
    {
        m_logger->error("Log path too long");
        return;
    }

    std::string episode_path_str = std::string(episode_path.data());

    if (!std::filesystem::create_directories(episode_path_str))
    {
        m_logger->error("Failed to create episode directory");
    }
    else
    {
        m_logger->debug("Created episode directory");
    }

    m_accelerometer_file = fopen((episode_path_str + "/acceleration").c_str(), "w");
    if (m_accelerometer_file == nullptr)
    {
        m_logger->error("Failed to open accelerometer file");
        return;
    }
    m_logger->debug("Opened accelerometer file");

    m_barometer_file = fopen((episode_path_str + "/pressure").c_str(), "w");
    if (m_barometer_file == nullptr)
    {
        m_logger->error("Failed to open barometer file");
        return;
    }
    m_logger->debug("Opened barometer file");

    m_gnss_file = fopen((episode_path_str + "/gnss").c_str(), "w");
    if (m_gnss_file == nullptr)
    {
        m_logger->error("Failed to open gnss file");
        return;
    }
    m_logger->debug("Opened gnss file");

    m_thread = std::thread(static_thread_entry, this);
}

void DataLogger::queue_acceleration_sample(AccelerometerSample& sample)
{
    std::lock_guard<std::mutex> lk(m_accelerometer_mutex);
    m_accelerometer_queue.push(sample);
}

void DataLogger::queue_barometer_sample(BarometerSample& sample)
{
    std::lock_guard<std::mutex> lk(m_barometer_mutex);
    m_barometer_queue.push(sample);
}

void DataLogger::queue_gnss_sample(std::string& sample)
{
    std::lock_guard<std::mutex> lk(m_gnss_mutex);
    m_gnss_queue.push(sample);
}

void DataLogger::thread_entry()
{
    bool flush = false;
    while (!m_accelerometer_queue.empty())
    {
        auto sample = m_accelerometer_queue.front();
        {
            std::lock_guard<std::mutex> lk(m_accelerometer_mutex);
            m_accelerometer_queue.pop();
        }

        if (m_accelerometer_file != nullptr)
        {
            ssize_t res = fwrite(&sample, sizeof(sample), 1, m_accelerometer_file);

            if (res < 0)
            {
                m_logger->error("Failed to write accelerometer sample to file: %d", res);
            }
            else if (res < sizeof(sample))
            {
                m_logger->error("Failed to write entire accelerometer sample to file. Wrote %d", res);
            }
            else
            {
                flush = true;
            }
        }
    }

    if (flush)
    {
        fflush(m_accelerometer_file);
    }

    flush = false;
    while (!m_barometer_queue.empty())
    {
        auto sample = m_barometer_queue.front();
        {
            std::lock_guard<std::mutex> lk(m_barometer_mutex);
            m_barometer_queue.pop();
        }

        if (m_barometer_file != nullptr)
        {
            ssize_t res = fwrite(&sample, sizeof(sample), 1, m_barometer_file);

            if (res < 0)
            {
                m_logger->error("Failed to write barometer sample to file: %d", res);
            }
            else if (res < sizeof(sample))
            {
                m_logger->error("Failed to write entire barometer sample to file. Wrote %d", res);
            }
            else
            {
                flush = true;
            }
        }
    }

    if (flush)
    {
        fflush(m_barometer_file);
    }

    flush = false;
    while (!m_gnss_queue.empty())
    {
        auto sample = m_gnss_queue.front();
        {
            std::lock_guard<std::mutex> lk(m_gnss_mutex);
            m_gnss_queue.pop();
        }

        if (m_gnss_file != nullptr)
        {
            ssize_t res = fwrite(sample.c_str(), sample.length(), 1, m_gnss_file);

            if (res < 0)
            {
                m_logger->error("Failed to write gnss sample to file: %d", res);
            }
            else if (res < sizeof(sample))
            {
                m_logger->error("Failed to write entire gnss sample to file. Wrote %d", res);
            }
            else
            {
                flush = true;
            }

            char newline = '\n';
            res = fwrite(&newline, sizeof(newline), 1, m_gnss_file);

            if (res < sizeof(newline))
            {
                m_logger->error("Failed to write newline to gnss file: %d", res);
            }
        }
    }

    if (flush)
    {
        fflush(m_gnss_file);
    }

    sleep(1);
}

void DataLogger::static_thread_entry(DataLogger* dataLogger)
{
    dataLogger->thread_entry();
}