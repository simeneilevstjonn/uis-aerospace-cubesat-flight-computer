#include "barometer.h"
#include "i2c.h"

Barometer::Barometer()
{
    /* Initialize mems driver interface */
    m_dev_ctx.write_reg = platform_write;
    m_dev_ctx.read_reg = platform_read;
    m_dev_ctx.mdelay = platform_delay;
    m_dev_ctx.handle = &LPS22HB_ADDRESS;

    /* Check device ID */
    uint8_t id;
    lps22hb_device_id_get(&m_dev_ctx, &id);
    if (id != LPS22HB_ID)
    {
        throw -1;
    }

    /* Restore default configuration */
    uint8_t status;
    lps22hb_reset_set(&m_dev_ctx, PROPERTY_ENABLE);
    do {
        lps22hb_reset_get(&m_dev_ctx, &status);
    } while (status);

    lps22hb_low_power_set(&m_dev_ctx, PROPERTY_DISABLE);

    lps22hb_data_rate_set(&m_dev_ctx, LPS22HB_ODR_10_Hz);

    lps22hb_fifo_watermark_set(&m_dev_ctx, 10);
    lps22hb_fifo_mode_set(&m_dev_ctx, LPS22HB_STREAM_MODE);
    lps22hb_fifo_set(&m_dev_ctx, PROPERTY_ENABLE);
}

std::queue<BarometerSample>& Barometer::get_fifo()
{
    return m_fifo;
}

int Barometer::fifo_read()
{
    uint8_t val, i;
    /* Read output only if new value is available */
    lps22hb_fifo_fth_flag_get(&m_dev_ctx, &val);

    if (val)
    {
        lps22hb_fifo_data_level_get(&m_dev_ctx, &val);

        for (i = 0; i < val; i++)
        {
            /* Read pressure */
            uint32_t pressure = 0;
            lps22hb_pressure_raw_get(&m_dev_ctx, &pressure);

            // Might need to read temperature to clear data from FIFO
            uint16_t temp;
            lps22hb_temperature_raw_get(&m_dev_ctx, &temp);

            float sample = lps22hb_from_lsb_to_hpa(pressure);

            m_fifo.push(sample);
        }
    }

    return val;
}