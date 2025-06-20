/**
 * @file sensor_i2c.h
 * @brief Temperature sensor interface over I2C.
 *
 * Provides a function to read the current temperature from
 * the TC74 I2C digital temperature sensor.
 *
 * @authors Duarte Caldeira, Tiago Saraiva
 * 
 */

#ifndef SENSOR_I2C_H
#define SENSOR_I2C_H

/**
 * @brief Initializes and reads temperature from the sensor.
 *
 * @return Temperature in degrees Celsius on success, or -1 on failure.
 */
int sensor_init_read(void);

#endif // SENSOR_I2C_H