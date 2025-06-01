#pragma once
#include <queue>
#include <cstdint>
#include "lps22hb_reg.h"
#include "logger.h"

#pragma pack(1)
struct BarometerSample
{
    uint32_t pressure;
    int16_t temperature;
};
#pragma pack()

class Barometer
{
  public:
    Barometer(Logger* logger);
    int fifo_read();
    std::queue<BarometerSample>& get_fifo();

  private:
    uint8_t LPS22HB_ADDRESS = LPS22HB_I2C_ADD_H >> 1;

    std::queue<BarometerSample> m_fifo = std::queue<BarometerSample>();
    stmdev_ctx_t m_dev_ctx;

    Logger* m_logger;
};