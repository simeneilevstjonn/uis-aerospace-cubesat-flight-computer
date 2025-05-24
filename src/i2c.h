#pragma once
#include <cstdint>

/** Please note that is MANDATORY: return 0 -> no Error.**/
int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len);
int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);

/** Optional (may be required by driver) **/
void platform_delay(uint32_t millisec);
