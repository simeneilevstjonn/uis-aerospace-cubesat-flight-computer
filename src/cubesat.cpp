#include "i2c.h"
#include "lis2dw12_reg.h"
#include <cstdio>
#include <cstring>

#define BOOT_TIME 20 // ms

void lis2dw12_read_data_polling(void)
{
    static int16_t data_raw_acceleration[3];
    static float_t acceleration_mg[3];
    static uint8_t whoamI, rst;

    uint8_t addr = 0x19;
    /* Initialize mems driver interface */
    stmdev_ctx_t dev_ctx;
    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;
    dev_ctx.mdelay = platform_delay;
    dev_ctx.handle = &addr;
    /* Wait sensor boot time */
    platform_delay(BOOT_TIME);
    /* Check device ID */
    lis2dw12_device_id_get(&dev_ctx, &whoamI);

    if (whoamI != LIS2DW12_ID)
    {
        printf("Device not found\n");
        return;
    }

    /* Restore default configuration */
    lis2dw12_reset_set(&dev_ctx, PROPERTY_ENABLE);

    do {
        lis2dw12_reset_get(&dev_ctx, &rst);
    } while (rst);

    /* Enable Block Data Update */
    lis2dw12_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
    /* Set full scale */
    // lis2dw12_full_scale_set(&dev_ctx, LIS2DW12_8g);
    lis2dw12_full_scale_set(&dev_ctx, LIS2DW12_2g);
    /* Configure filtering chain
     * Accelerometer - filter path / bandwidth
     */
    lis2dw12_filter_path_set(&dev_ctx, LIS2DW12_LPF_ON_OUT);
    lis2dw12_filter_bandwidth_set(&dev_ctx, LIS2DW12_ODR_DIV_4);
    /* Configure power mode */
    lis2dw12_power_mode_set(&dev_ctx, LIS2DW12_HIGH_PERFORMANCE);
    // lis2dw12_power_mode_set(&dev_ctx, LIS2DW12_CONT_LOW_PWR_LOW_NOISE_12bit);
    /* Set Output Data Rate */
    lis2dw12_data_rate_set(&dev_ctx, LIS2DW12_XL_ODR_25Hz);

    /* Read samples in polling mode (no int) */
    while (1)
    {
        uint8_t reg;
        /* Read output only if new value is available */
        lis2dw12_flag_data_ready_get(&dev_ctx, &reg);

        if (reg)
        {
            /* Read acceleration data */
            memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
            lis2dw12_acceleration_raw_get(&dev_ctx, data_raw_acceleration);
            // acceleration_mg[0] = lis2dw12_from_fs8_lp1_to_mg(data_raw_acceleration[0]);
            // acceleration_mg[1] = lis2dw12_from_fs8_lp1_to_mg(data_raw_acceleration[1]);
            // acceleration_mg[2] = lis2dw12_from_fs8_lp1_to_mg(data_raw_acceleration[2]);
            acceleration_mg[0] = lis2dw12_from_fs2_to_mg(data_raw_acceleration[0]);
            acceleration_mg[1] = lis2dw12_from_fs2_to_mg(data_raw_acceleration[1]);
            acceleration_mg[2] = lis2dw12_from_fs2_to_mg(data_raw_acceleration[2]);
            printf("Acceleration [mg]:%4.2f\t%4.2f\t%4.2f\r\n", acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);
        }
    }
}

int main()
{
    if (auto res = i2c_init(); res)
    {
        return res;
    }

    lis2dw12_read_data_polling();
}