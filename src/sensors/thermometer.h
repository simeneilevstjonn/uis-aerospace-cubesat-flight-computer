#pragma once
#include <queue>
#include <cstdint>
#include <fstream>

using TemperatureSample = float;

class Thermometer
{
  public:
    Thermometer();

    int fifo_read();
    std::queue<TemperatureSample>& get_fifo();

  private:
    std::queue<TemperatureSample> m_fifo = std::queue<TemperatureSample>();
    std::ifstream m_stream;
};