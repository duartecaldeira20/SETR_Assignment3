#include <zephyr/sys/printk.h>
#include <zephyr/drivers/i2c.h>
#include "sensor_i2c.h"

#define ERR_FATAL -1
#define TC74_CMD_RTR 0x00
#define I2C0_NID DT_NODELABEL(tc74sensor)
static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C0_NID);

int sensor_init_read(void)
{
    int ret = 0;
    uint8_t temp = 0;
    uint8_t cmd = TC74_CMD_RTR;

    if (!device_is_ready(dev_i2c.bus)) {
        printk("I2C bus %s is not ready!\n", dev_i2c.bus->name);
        return ERR_FATAL;
    }

    ret = i2c_write_dt(&dev_i2c, &cmd, 1);
    if (ret != 0) {
        //printk("Failed to write RTR command to I2C addr 0x%x, reg 0x%x\n", dev_i2c.addr, TC74_CMD_RTR);
        return ERR_FATAL;
    }

    ret = i2c_read_dt(&dev_i2c, &temp, sizeof(temp));
    if (ret != 0) {
        //printk("Failed to read from I2C addr 0x%x, reg 0x%x\n", dev_i2c.addr, TC74_CMD_RTR);
        return ERR_FATAL;
    }

    return (int)temp;
}