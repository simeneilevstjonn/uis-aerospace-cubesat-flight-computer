#pragma once
#include <queue>
#include <cstdint>
#include "a3g4250d_reg.h"

struct GyroscopeSample
{
    float x;
    float y;
    float z;
};

class Gyroscope
{
  public:
    Gyroscope();
    int fifo_read();
    std::queue<GyroscopeSample>& get_fifo();

  private:
    uint8_t A3G4250D_ADDRESS = A3G4250D_I2C_ADD_L >> 1;

    std::queue<GyroscopeSample> m_fifo = std::queue<GyroscopeSample>();
    stmdev_ctx_t m_dev_ctx;
};