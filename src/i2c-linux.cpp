#include "i2c.h"
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <fcntl.h>
#include <iostream>
#include <sys/ioctl.h>
#include <cstring>

int m_adapter_file;
int m_last_used_reg = -1;

int32_t i2c_init()
{
    m_adapter_file = open("/dev/i2c-1", O_RDWR);

    if (m_adapter_file < 0)
    {
        return -1;
    }

    return 0;
}

int32_t slave_set(uint8_t address)
{
    if (m_last_used_reg != address)
    {
        if (auto res = ioctl(m_adapter_file, I2C_SLAVE, address); res)
        {
            return res;
        }
    }

    m_last_used_reg = address;
    return 0;
}

/** Please note that is MANDATORY: return 0 -> no Error.**/
int32_t platform_write(void* handle, uint8_t reg, const uint8_t* bufp, uint16_t len)
{
    uint8_t address = *(uint8_t*)handle;
    if (auto err = slave_set(address); err)
    {
        return err;
    }

    uint8_t* data = new uint8_t[len + sizeof(reg)];
    if (data == nullptr)
    {
        return -1;
    }

    data[0] = reg;
    memcpy(data + 1, bufp, len);

    if (auto res = write(m_adapter_file, data, len + sizeof(reg)); res != len + sizeof(reg))
    {
        delete[] data;
        return res;
    }

    delete[] data;
    return 0;
}

int32_t platform_read(void* handle, uint8_t reg, uint8_t* bufp, uint16_t len)
{
    uint8_t address = *(uint8_t*)handle;
    if (auto err = slave_set(address); err)
    {
        return err;
    }

    if (auto res = write(m_adapter_file, &reg, sizeof(reg)); res != sizeof(reg))
    {
        return res;
    }

    if (auto res = read(m_adapter_file, bufp, len); res != len)
    {
        return res;
    }

    return 0;
}

/** Optional (may be required by driver) **/
void platform_delay(uint32_t millisec)
{
    usleep(1000 * millisec);
}
