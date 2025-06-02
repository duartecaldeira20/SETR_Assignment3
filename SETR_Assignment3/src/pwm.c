#include "pwm.h"
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>

#define PWM_NODE DT_NODELABEL(pwm_led2)
static const struct pwm_dt_spec pwm_led = PWM_DT_SPEC_GET(PWM_NODE);

int pwm_ctrl_init(void)
{
    if (!device_is_ready(pwm_led.dev)) {
        printk("PWM device not ready!\n");
        return -1;
    }

    // Iniciar com 0% duty cycle
    return pwm_ctrl_set_duty_cycle_percent(0);
}

int pwm_ctrl_set_duty_cycle_percent(uint8_t percent)
{
    if (percent > 100) percent = 100;

    uint32_t pulse = (PWM_PERIOD_NS * percent) / 100;
    return pwm_set_dt(&pwm_led, PWM_PERIOD_NS, pulse);
}