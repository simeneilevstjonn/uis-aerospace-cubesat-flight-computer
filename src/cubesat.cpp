#include "accelerometer.h"
#include "gyroscope.h"
#include "i2c.h"
#include <iostream>

int main()
{
    i2c_init();

    auto gyro = Gyroscope();
    auto acc = Accelerometer();

    while (true)
    {
        gyro.fifo_read();
        acc.fifo_read();

        auto& gyro_fifo = gyro.get_fifo();
        while (!gyro_fifo.empty())
        {
            auto sample = gyro_fifo.front();
            gyro_fifo.pop();
            std::cout << "Gyro sample " << sample.x << " " << sample.y << " " << sample.z << "\n";
        }

        auto& acc_fifo = acc.get_fifo();
        while (!acc_fifo.empty())
        {
            auto sample = acc_fifo.front();
            acc_fifo.pop();
            std::cout << "Acceleration sample " << sample.x << " " << sample.y << " " << sample.z << "\n";
        }

        // platform_delay(10);
    }
}