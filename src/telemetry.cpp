#include "telemetry.h"
#include "unistd.h"
#include <algorithm>

Telemetry::Telemetry(Logger* logger)
{
    m_logger = logger;

    // TODO reset modem

    if (!m_rf95.init())
    {
        m_logger->error("Telemetry init failed!");
        return;
    }
    m_logger->debug("Telemetry module inited");

    if (!m_rf95.setFrequency(FREQUENCY))
    {
        m_logger->error("Frequency assignment failed.");
        return;
    }
    m_logger->debug("Set frequency to %f MHz", FREQUENCY);

    if (!m_rf95.setTxPower(TRANSMIT_POWER, false))
    {
        m_logger->error("Transmit power assignment failed.");
        return;
    }
    m_logger->debug("Set transmit power to %d dBm", TRANSMIT_POWER);

    m_thread = std::thread(static_thread_entry, this);
}

bool Telemetry::send_blocking(const void* data, size_t len)
{
    if (len > RH_RF95_MAX_MESSAGE_LEN)
    {
        m_logger->error("Attempted to send %ld bytes while telemetry module only supports %d", len, RH_RF95_MAX_MESSAGE_LEN);
        return false;
    }

    if (!m_rf95.send(data, len))
    {
        m_logger->error("Failed to send telemtry data");
        return false;
    }

    m_rf95.waitPacketSent();
}

void Telemetry::static_thread_entry(Telemetry* telemetry)
{
    telemetry->thread_entry();
}

void Telemetry::thread_entry()
{
    while (true)
    {
        while (!m_transmit_queue.empty())
        {
            auto* to_send = m_transmit_queue.front();
            {
                std::lock_guard<std::mutex> lk(m_queue_mutex);
                m_transmit_queue.pop();
            }

            if (to_send->size())
            {
                send_blocking(to_send->data(), to_send->size());
            }

            delete to_send;
        }

        usleep(10 * 1000);
    }
}

void Telemetry::new_array_for_type(PacketType type)
{
    m_constructing_packet_buffers[index] = new vector<uint8_t>();
    auto* buf = m_constructing_packet_buffers[index];
    auto seq = m_sequence_numbers[index]++;

    buf->push_back(static_cast<int>(type));
    buf->push_back(seq & 0xFF);
    buf->push_back((seq >> 8) & 0xFF);
    buf->push_back((seq >> 16) & 0xFF);
    buf->push_back((seq >> 24) & 0xFF);
}

void Telemetry::enqueue_data(PacketType type, const void* data, size_t len)
{
    auto index = static_cast<int>(type);

    if (m_constructing_packet_buffers[index] == nullptr)
    {
        new_array_for_type(type);
    }

    auto* buf = m_constructing_packet_buffers[index];

    if (buf->size + len > RH_RF95_MAX_MESSAGE_LEN)
    {
        std::lock_guard<std::mutex> lk(m_queue_mutex);
        m_queue.push(buf);

        new_array_for_type(type);
        buf = m_constructing_packet_buffers[index];
    }

    for (size_t i = 0; i < len; i++)
    {
        buf->push_back(static_cast<const uint8_t*>(data)[i]);
    }
}

void Telemetry::enqueue_acceleration(AccelerometerSample& sample)
{
    enqueue_data(PacketType::Acceleration, &sample, sizeof(sample))
}

void Telemetry::enqueue_pressure(BarometerSample& sample)
{
    enqueue_data(PacketType::Pressure, &sample, sizeof(sample))
}

void Telemetry::enqueue_gnss(std::string sample)
{
    // Add delimiter. Since parameter is not passed by reference, this should be done to a copy
    sample += "\n";
    enqueue_data(PacketType::Gnss, sample.c_str(), sample.size())
}