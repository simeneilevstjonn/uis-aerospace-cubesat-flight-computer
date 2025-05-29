#pragma once
#include <queue>
#include <cstdint>
#include "lps22hb_reg.h"

using BarometerSample = float;

class Barometer
{
  public:
    Barometer();
    int fifo_read();
    std::queue<BarometerSample>& get_fifo();

  private:
    uint8_t LPS22HB_ADDRESS = LPS22HB_I2C_ADD_H >> 1;

    std::queue<BarometerSample> m_fifo = std::queue<BarometerSample>();
    stmdev_ctx_t m_dev_ctx;
};