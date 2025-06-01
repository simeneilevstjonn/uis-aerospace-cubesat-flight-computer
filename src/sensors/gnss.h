#pragma once
#include <array>
#include <cstdint>
#include <cstddef>
#include <queue>
#include <string>
#include "logger.h"

class GNSS
{
  public:
    GNSS(Logger* logger);

    void tick();

    std::queue<std::string>& get_fifo();

  private:
    int m_file;
    std::array<uint8_t, 200> m_packet;
    size_t m_packet_index = 0;
    Logger* m_logger;

    std::queue<std::string> m_fifo;

    void decode_bytes(const uint8_t* data, size_t len);
    void process_packet(const uint8_t* data, size_t len);
};