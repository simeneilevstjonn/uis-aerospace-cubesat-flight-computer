#include "accelerometer.h"
#include "i2c.h"
#include <cstring>

Accelerometer::Accelerometer()
{
    m_dev_ctx.write_reg = platform_write;
    m_dev_ctx.read_reg = platform_read;
    m_dev_ctx.mdelay = platform_delay;
    // Handle should be the I2C address
    m_dev_ctx.handle = (void*)&LIS2DW12_ADDRESS;

    uint8_t whoamI;
    /* Check device ID */
    lis2dw12_device_id_get(&m_dev_ctx, &whoamI);

    if (whoamI != LIS2DW12_ID)
    {
        throw -1;
    }

    /* Restore default configuration */
    lis2dw12_reset_set(&m_dev_ctx, PROPERTY_ENABLE);

    uint8_t rst;
    do {
        lis2dw12_reset_get(&m_dev_ctx, &rst);
    } while (rst);

    /* Enable Block Data Update */
    lis2dw12_block_data_update_set(&m_dev_ctx, PROPERTY_ENABLE);
    /* Set full scale */
    // lis2dw12_full_scale_set(&m_dev_ctx, LIS2DW12_8g);
    lis2dw12_full_scale_set(&m_dev_ctx, LIS2DW12_2g);
    /* Configure filtering chain
     * Accelerometer - filter path / bandwidth
     */
    lis2dw12_filter_path_set(&m_dev_ctx, LIS2DW12_LPF_ON_OUT);
    lis2dw12_filter_bandwidth_set(&m_dev_ctx, LIS2DW12_ODR_DIV_4);
    /* Configure FIFO */
    lis2dw12_fifo_watermark_set(&m_dev_ctx, 10);
    lis2dw12_fifo_mode_set(&m_dev_ctx, LIS2DW12_STREAM_MODE);
    /* Configure power mode */
    lis2dw12_power_mode_set(&m_dev_ctx, LIS2DW12_HIGH_PERFORMANCE_LOW_NOISE);
    // lis2dw12_power_mode_set(&m_dev_ctx, LIS2DW12_CONT_LOW_PWR_LOW_NOISE_12bit);
    /* Set Output Data Rate */
    lis2dw12_data_rate_set(&m_dev_ctx, LIS2DW12_XL_ODR_25Hz);
}

int Accelerometer::fifo_read()
{
    uint8_t val, i;
    /* Read output only if new value is available */
    lis2dw12_fifo_wtm_flag_get(&m_dev_ctx, &val);

    if (val)
    {
        lis2dw12_fifo_data_level_get(&m_dev_ctx, &val);

        for (i = 0; i < val; i++)
        {
            /* Read acceleration data */
            int16_t data_raw_acceleration[3];
            memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
            lis2dw12_acceleration_raw_get(&m_dev_ctx, data_raw_acceleration);

            AccelerometerSample sample;

            sample.x = lis2dw12_from_fs2_to_mg(data_raw_acceleration[0]);
            sample.y = lis2dw12_from_fs2_to_mg(data_raw_acceleration[1]);
            sample.z = lis2dw12_from_fs2_to_mg(data_raw_acceleration[2]);

            m_fifo.push(sample);
        }
    }

    return val;
}

std::queue<AccelerometerSample>& Accelerometer::get_fifo()
{
    return m_fifo;
}