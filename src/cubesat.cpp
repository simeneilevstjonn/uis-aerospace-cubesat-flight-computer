#include "accelerometer.h"
#include "thermometer.h"
#include "barometer.h"
#include "i2c.h"
#include "gnss.h"
#include "logger.h"
#include "log_backend.h"
#include <iostream>

int main()
{
    i2c_init();

    LogBackend log_backend;
    log_backend.register_stdout(LogLevel::Debug);
    log_backend.enable();

    Logger main_logger = Logger("main", &log_backend);
    main_logger.info("CubeSat application starting");

    auto acc_logger = Logger("Accelerometer", &log_backend);
    auto thermo_logger = Logger("Thermometer", &log_backend);
    auto baro_logger = Logger("Barometer", &log_backend);
    auto gnss_logger = Logger("GNSS", &log_backend);

    auto acc = Accelerometer(&acc_logger);
    auto thermo = Thermometer(&thermo_logger);
    auto baro = Barometer(&baro_logger);
    auto gnss = GNSS(&gnss_logger);

    while (true)
    {
        int acc_read = acc.fifo_read();
        int baro_read = baro.fifo_read();
        gnss.tick();

        if (acc_read || baro_read)
        {
            main_logger.debug("Read %d samples for acceleration FIFO. %d samples from barometer FIFO.", acc_read, baro_read);
        }

        auto& acc_fifo = acc.get_fifo();
        while (!acc_fifo.empty())
        {
            auto sample = acc_fifo.front();
            acc_fifo.pop();
        }

        auto& baro_fifo = baro.get_fifo();
        while (!baro_fifo.empty())
        {
            auto sample = baro_fifo.front();
            baro_fifo.pop();
        }

        auto& gnss_fifo = gnss.get_fifo();
        int gnss_count = 0;
        while (!gnss_fifo.empty())
        {
            auto packet = gnss_fifo.front();
            gnss_fifo.pop();
            gnss_count++;
        }

        if (gnss_count)
        {
            main_logger.debug("Read %d frames from GNSS.", gnss_count);
        }

        platform_delay(100);
    }
}