#pragma once
#include <queue>
#include <cstdint>
#include <fstream>

using TemperatureSample = float;

class Thermometer
{
  public:
    Thermometer();

    TemperatureSample current();

  private:
    std::ifstream m_stream;
};