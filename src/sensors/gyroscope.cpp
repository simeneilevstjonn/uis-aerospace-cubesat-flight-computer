#include "gyroscope.h"
#include <cstring>
#include "i2c.h"

Gyroscope::Gyroscope()
{
    /* Initialize mems driver interface */
    m_dev_ctx.write_reg = platform_write;
    m_dev_ctx.read_reg = platform_read;
    m_dev_ctx.mdelay = platform_delay;
    m_dev_ctx.handle = &A3G4250D_ADDRESS;
    /* Check device ID */
    uint8_t whoamI;
    a3g4250d_device_id_get(&m_dev_ctx, &whoamI);

    if (whoamI != A3G4250D_ID)
    {
        throw -1;
    }

    // /* Set FIFO watermark to 10 samples */
    // a3g4250d_fifo_watermark_set(&m_dev_ctx, 10);
    // /* Set FIFO mode to FIFO MODE */
    // a3g4250d_fifo_mode_set(&m_dev_ctx, A3G4250D_FIFO_STREAM_MODE);
    // /* Enable FIFO */
    // a3g4250d_fifo_enable_set(&m_dev_ctx, PROPERTY_ENABLE);
    /* Set Output Data Rate */
    a3g4250d_data_rate_set(&m_dev_ctx, A3G4250D_ODR_100Hz);
}

int Gyroscope::fifo_read()
{
    /* Read angular rate data */
    int16_t data_raw_angular_rate[3];
    memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
    if (a3g4250d_angular_rate_raw_get(&m_dev_ctx, data_raw_angular_rate))
    {
        GyroscopeSample sample;
        sample.x = a3g4250d_from_fs245dps_to_mdps(data_raw_angular_rate[0]);
        sample.y = a3g4250d_from_fs245dps_to_mdps(data_raw_angular_rate[1]);
        sample.z = a3g4250d_from_fs245dps_to_mdps(data_raw_angular_rate[2]);

        m_fifo.push(sample);
        return 1;
    }

    return 0;
}

std::queue<GyroscopeSample>& Gyroscope::get_fifo()
{
    return m_fifo;
}