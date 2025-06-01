#pragma once
#include <queue>
#include <cstdint>
#include "lis2dw12_reg.h"
#include "logger.h"

#pragma pack(1)
struct AccelerometerSample
{
    int16_t x;
    int16_t y;
    int16_t z;
};
#pragma pack()

class Accelerometer
{
  public:
    Accelerometer(Logger* logger);
    int fifo_read();
    std::queue<AccelerometerSample>& get_fifo();

  private:
    uint8_t LIS2DW12_ADDRESS = LIS2DW12_I2C_ADD_H >> 1;

    std::queue<AccelerometerSample> m_fifo = std::queue<AccelerometerSample>();
    stmdev_ctx_t m_dev_ctx;

    Logger* m_logger;
};