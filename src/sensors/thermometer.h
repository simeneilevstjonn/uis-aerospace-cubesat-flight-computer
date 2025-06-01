#pragma once
#include <queue>
#include <cstdint>
#include <fstream>
#include <string>
#include "logger.h"

using TemperatureSample = float;

class Thermometer
{
  public:
    Thermometer(Logger* logger);

    TemperatureSample current();

  private:
    std::ifstream m_stream;
    Logger* m_logger;

    const char* THERMO_PATH = "/sys/bus/i2c/devices/1-0048/hwmon/hwmon1/temp1_input";
};