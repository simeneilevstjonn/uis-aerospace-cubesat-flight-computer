#include "i2c.h"
#include "lis2dw12_reg.h"
#include "a3g4250d_reg.h"
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

void a3g4250d_read_data_polling(void)
{
    static int16_t data_raw_angular_rate[3];
    static float_t angular_rate_mdps[3];
    static uint8_t whoamI;
    uint8_t addr = 0x68;
    stmdev_ctx_t dev_ctx;
    /* Uncomment to use interrupts on drdy */
    // a3g4250d_int2_route_t int2_reg;
    /* Initialize mems driver interface */
    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;
    dev_ctx.mdelay = platform_delay;
    dev_ctx.handle = &addr;
    /* Wait sensor boot time */
    platform_delay(BOOT_TIME);
    /* Check device ID */
    a3g4250d_device_id_get(&dev_ctx, &whoamI);

    if (whoamI != A3G4250D_ID)
    {
        while (1); /*manage here device not found */
    }

    /* Configure filtering chain -  Gyroscope - High Pass */
    a3g4250d_filter_path_set(&dev_ctx, A3G4250D_LPF1_HP_ON_OUT);
    a3g4250d_hp_bandwidth_set(&dev_ctx, A3G4250D_HP_LEVEL_3);
    /* Uncomment to use interrupts on drdy */
    // a3g4250d_pin_int2_route_get(&dev_ctx, &int2_reg);
    // int2_reg.i2_drdy = PROPERTY_ENABLE;
    // a3g4250d_pin_int2_route_set(&dev_ctx, int2_reg);
    /* Set Output Data Rate */
    a3g4250d_data_rate_set(&dev_ctx, A3G4250D_ODR_100Hz);

    /* Read samples in polling mode (no int) */
    int nodata = 0;
    while (1)
    {
        uint8_t reg;
        /* Read output only if new value is available */
        a3g4250d_flag_data_ready_get(&dev_ctx, &reg);

        if (reg)
        {
            /* Read angular rate data */
            memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
            a3g4250d_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate);
            angular_rate_mdps[0] = a3g4250d_from_fs245dps_to_mdps(data_raw_angular_rate[0]);
            angular_rate_mdps[1] = a3g4250d_from_fs245dps_to_mdps(data_raw_angular_rate[1]);
            angular_rate_mdps[2] = a3g4250d_from_fs245dps_to_mdps(data_raw_angular_rate[2]);
            printf("Angular Rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n", angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
            nodata = 0;
        }
        else
        {
            nodata++;
            if (nodata % 1000 == 0)
            {
                printf("No data! %d\n", nodata);
            }
        }
    }
}

void a3g4250d_fifo_read(void)
{
    static int16_t data_raw_angular_rate[3];
    static float_t angular_rate_mdps[3];
    static uint8_t whoamI;
    uint8_t addr = 0x68;
    stmdev_ctx_t dev_ctx;
    /* Uncomment to use interrupts on drdy */
    // a3g4250d_int2_route_t int2_reg;
    /* Initialize mems driver interface */
    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;
    dev_ctx.mdelay = platform_delay;
    dev_ctx.handle = &addr;
    /* Initialize platform specific hardware */
    /* Wait sensor boot time */
    platform_delay(BOOT_TIME);
    /* Check device ID */
    a3g4250d_device_id_get(&dev_ctx, &whoamI);

    if (whoamI != A3G4250D_ID)
    {
        while (1); /*manage here device not found */
    }

    /* Set FIFO watermark to 10 samples */
    a3g4250d_fifo_watermark_set(&dev_ctx, 10);
    /* Set FIFO mode to FIFO MODE */
    a3g4250d_fifo_mode_set(&dev_ctx, A3G4250D_FIFO_STREAM_MODE);
    /* Enable FIFO */
    a3g4250d_fifo_enable_set(&dev_ctx, PROPERTY_ENABLE);
    /* Uncomment to configure watermark interrupt on INT2 */
    // a3g4250d_pin_int2_route_get(&dev_ctx, &int2_route);
    // int2_route.i2_wtm = PROPERTY_ENABLE;
    // a3g4250d_pin_int2_route_set(&dev_ctx, int2_route);
    /* Set Output Data Rate */
    a3g4250d_data_rate_set(&dev_ctx, A3G4250D_ODR_100Hz);

    /* Wait Events Loop */
    int nodata = 0;
    while (1)
    {
        uint8_t flags;
        uint8_t num = 0;
        /* Read watermark interrupt flag */
        a3g4250d_fifo_wtm_flag_get(&dev_ctx, &flags);

        if (flags)
        {
            /* Read how many samples in fifo */
            a3g4250d_fifo_data_level_get(&dev_ctx, &num);

            while (num-- > 0)
            {
                /* Read angular rate data */
                memset(data_raw_angular_rate, 0x00, 3 * sizeof(int16_t));
                a3g4250d_angular_rate_raw_get(&dev_ctx, data_raw_angular_rate);
                angular_rate_mdps[0] = a3g4250d_from_fs245dps_to_mdps(data_raw_angular_rate[0]);
                angular_rate_mdps[1] = a3g4250d_from_fs245dps_to_mdps(data_raw_angular_rate[1]);
                angular_rate_mdps[2] = a3g4250d_from_fs245dps_to_mdps(data_raw_angular_rate[2]);
                printf("Angular Rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n", angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
            }

            nodata = 0;
        }
        else
        {
            nodata++;
            if (nodata % 1000 == 0)
            {
                printf("No data! %d\n", nodata);
            }
        }
    }
}

int main()
{
    if (auto res = i2c_init(); res)
    {
        return res;
    }

    a3g4250d_fifo_read();
}