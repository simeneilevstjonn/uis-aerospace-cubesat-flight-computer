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
            std::cout << "Gyro sample " << sample.x << " " << sample.y << " " << sample.z << std::endl;
        }

        auto& acc_fifo = gyro.get_fifo();
        while (!gyro_fifo.empty())
        {
            auto sample = gyro_fifo.front();
            gyro_fifo.pop();
            std::cout << "Gyro sample " << sample.x << " " << sample.y << " " << sample.z << std::endl;
        }

        platform_delay(50);
    }
}