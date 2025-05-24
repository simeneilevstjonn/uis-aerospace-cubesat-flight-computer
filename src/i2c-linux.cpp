#include "i2c.h"
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

struct i2c_adapter *m_adapter = nullptr;

void i2c_init()
{
    m_adapter = i2c_get_adapter(1);
}

/** Please note that is MANDATORY: return 0 -> no Error.**/
int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len)
{
}

int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
}

/** Optional (may be required by driver) **/
void platform_delay(uint32_t millisec)
{
    usleep(1000 * millisec);
}
