/**
 * @file pwm.h
 * @brief PWM control interface for Smart Home Heater.
 *
 * Provides functions to initialize and configure PWM output
 * to control a heating element or fan with duty cycle.
 *
 * @authors Duarte Caldeira, Tiago Saraiva
 * 
 */

#ifndef PWM_H
#define PWM_H

#include <zephyr/drivers/pwm.h>

#define PWM_FREQ_HZ      100                                 /**< PWM frequency in Hz. */
#define PWM_PERIOD_NS    (1000000000UL / PWM_FREQ_HZ)        /**< PWM period in nanoseconds. */

/**
 * @brief Initializes the PWM hardware.
 *
 * @return 0 on success, negative value on failure.
 */
int pwm_init(void);

/**
 * @brief Sets the PWM duty cycle.
 *
 * @param percent Duty cycle as a percentage (0 to 100).
 * @return 0 on success, negative value on error.
 */
int pwm_set_dc(uint8_t percent);

#endif // PWM_H