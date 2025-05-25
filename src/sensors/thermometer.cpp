#include "thermometer.h"

Thermometer::Thermometer()
{
    m_stream.open("/sys/bus/i2c/devices/1-0048/hwmon/hwmon1/temp1_input", std::fstream::in);
}

std::queue<TemperatureSample>& Thermometer::get_fifo()
{
    return m_fifo;
}

int Thermometer::fifo_read()
{
    if (m_stream.eof())
    {
        return 0;
    }

    int32_t sample;
    m_stream >> sample;

    m_fifo.push(sample / 1e3);

    return 1;
}