#ifndef PWM_CTRL_H
#define PWM_CTRL_H

#include <zephyr/drivers/pwm.h>

#define PWM_FREQ_HZ     100
#define PWM_PERIOD_NS   (1000000000UL / PWM_FREQ_HZ) // 10ms = 100Hz

int pwm_ctrl_init(void);
int pwm_ctrl_set_duty_cycle_percent(uint8_t percent); // 0-100%

#endif // PWM_CTRL_H