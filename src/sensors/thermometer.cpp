#include "thermometer.h"

Thermometer::Thermometer(Logger* logger)
{
    m_logger = logger;

    m_stream.open(THERMO_PATH, std::fstream::in);
}

TemperatureSample Thermometer::current()
{
    m_stream.seekg(0);

    int32_t sample;
    m_stream >> sample;

    return sample / 1e3;
}