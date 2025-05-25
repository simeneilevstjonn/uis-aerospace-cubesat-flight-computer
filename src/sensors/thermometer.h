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

    static const char* THERMO_PATH = "/sys/bus/i2c/devices/1-0048/hwmon/hwmon1/temp1_input";
};