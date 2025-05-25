#include "thermometer.h"

Thermometer::Thermometer()
{
    m_stream.open("/sys/bus/i2c/devices/1-0048/hwmon/hwmon1/temp1_input", std::fstream::in);
}

TemperatureSample Thermometer::current()
{
    m_stream.seekg(0);

    int32_t sample;
    m_stream >> sample;

    return sample / 1e3;
}