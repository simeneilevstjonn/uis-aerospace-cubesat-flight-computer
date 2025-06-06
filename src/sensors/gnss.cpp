#include "gnss.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

GNSS::GNSS(Logger* logger)
{
    m_logger = logger;

    m_file = open("/dev/ttyS0", O_RDONLY);

    if (m_file < 0)
    {
        m_logger->error("Failed to open GNSS file: %d", m_file);
        throw m_file;
    }
    m_logger->debug("Opened GNSS file");

    struct termios params = { 0 };
    cfsetispeed(&params, B9600);
    cfsetospeed(&params, B9600);
    cfmakeraw(&params);
    params.c_cflag |= CLOCAL;
    params.c_cflag |= CREAD;

    if (int res = tcsetattr(m_file, TCSANOW, &params); res)
    {
        m_logger->error("Failed to set serial port parameters: %d", res);
        throw res;
    }
}

void GNSS::tick()
{
    while (true)
    {
        std::array<uint8_t, 200> buffer;
        ssize_t res = read(m_file, buffer.data(), buffer.size());

        if (res < 0)
        {
            m_logger->error("Failed to read serial port: %d", res);
            break;
        }

        if (!res)
        {
            break;
        }

        decode_bytes(buffer.data(), res);

        if (res < buffer.size())
        {
            break;
        }
    }
}

void GNSS::decode_bytes(const uint8_t* data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        auto c = data[i];

        // If we have no bytes, we are looking for the '$' starting a packet
        if (m_packet_index == 0)
        {
            if (c == '$')
            {
                m_packet[m_packet_index++] = c;
            }
        }
        // Packets are end with newlines
        else if (c == '\n' || c == '\r')
        {
            if (m_packet_index)
            {
                // Add null terminator to allow parsing as string
                m_packet[m_packet_index] = 0;
                process_packet(m_packet.data(), m_packet_index);
                m_packet_index = 0;
            }
        }
        else if (m_packet_index == m_packet.size() - 1)
        {
            m_logger->error("Package too long! Possibly missed packet delimiters.");
            m_packet_index = 0;
        }
        else
        {
            m_packet[m_packet_index++] = c;
        }
    }
}

void GNSS::process_packet(const uint8_t* data, size_t len)
{
    // Consider parsing packet
    auto data_cc = reinterpret_cast<const char*>(data);
    m_fifo.push(std::string(data_cc));
}

std::queue<std::string>& GNSS::get_fifo()
{
    return m_fifo;
}