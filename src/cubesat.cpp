#include "accelerometer.h"
#include "gyroscope.h"
#include "thermometer.h"
#include "barometer.h"
#include "i2c.h"
#include <iostream>

int main()
{
    i2c_init();

    auto gyro = Gyroscope();
    auto acc = Accelerometer();
    auto thermo = Thermometer();
    auto baro = Barometer();

    while (true)
    {
        // int gyro_read = gyro.fifo_read();
        // int acc_read = acc.fifo_read();
        int baro_read = baro.fifo_read();

        // std::cout << "Read " << gyro_read << " samples from gyro FIFO, " << acc_read << " from acc_fifo\n";

        // auto& gyro_fifo = gyro.get_fifo();
        // while (!gyro_fifo.empty())
        // {
        //     auto sample = gyro_fifo.front();
        //     gyro_fifo.pop();
        //     std::cout << "Gyro sample " << sample.x << " " << sample.y << " " << sample.z << "\n";
        // }

        // auto& acc_fifo = acc.get_fifo();
        // while (!acc_fifo.empty())
        // {
        //     auto sample = acc_fifo.front();
        //     acc_fifo.pop();
        //     std::cout << "Acceleration sample " << sample.x << " " << sample.y << " " << sample.z << "\n";
        // }

        // std::cout << "Current temperature is " << thermo.current() << "\n";

        std::cout << "Read " << baro_read << " barometer samples:\n";

        auto& baro_fifo = baro.get_fifo();
        while (!baro_fifo.empty())
        {
            auto sample = baro_fifo.front();
            baro_fifo.pop();
            std::cout << "Read barometer sample " << sample << " hPa \n";
        }

        platform_delay(100);
    }
}