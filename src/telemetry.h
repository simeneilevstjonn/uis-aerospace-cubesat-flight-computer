#pragma once
#include "logger.h"
#include "RH_RF95.h"
#include <cstddef>
#include <queue>
#include <vector>
#include <cstdint>
#include <thread>
#include <mutex>
#include <array>
#include "accelerometer.h"
#include "barometer.h"
#include <string>

class Telemetry
{
  public:
    Telemetry(Logger* logger);

    enum class PacketType : uint8_t
    {
        Acceleration = 0,
        Pressure = 1,
        Gnss = 2
    };

    void enqueue_data(PacketType type, const void* data, size_t len);
    void enqueue_acceleration(AccelerometerSample& sample);
    void enqueue_pressure(BarometerSample& sample);
    // We want to make a copy, therefore not passing by reference
    void enqueue_gnss(std::string sample);

  private:
    Logger* m_logger;
    std::queue<std::vector<uint8_t>> m_transmit_queue;
    std::thread m_thread;
    std::mutex m_transmit_queue_mutex;

    static constexpr int RESET_PIN = 0;
    static constexpr int INTERRUPT_PIN = 0;
    static constexpr int TRANSMIT_POWER = 23;
    static constexpr float FREQUENCY = 437.0f;

    bool send_blocking(const void* data, size_t len);
    static void static_thread_entry(Telemetry* telemetry);
    void thread_entry();

    void new_array_for_type(PacketType type);

    RH_RF95 m_rf95;

    static constexpr int NUMBER_OF_PACKET_TYPES = static_cast<int>(PacketType::Gnss) + 1;

    std::array<std::vector<uint8_t>*, NUMBER_OF_PACKET_TYPES> m_constructing_packet_buffers = { 0 };
    std::array<uint32_t, NUMBER_OF_PACKET_TYPES> m_sequence_numbers = { 0 };
};