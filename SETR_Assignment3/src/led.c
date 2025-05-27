#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include "rtdb.h"

// Define os nós dos LEDs a partir dos aliases do device tree
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

static const struct gpio_dt_spec leds[] = 
{
    GPIO_DT_SPEC_GET(LED0_NODE, gpios),  // LED 0 → sistema ligado
    GPIO_DT_SPEC_GET(LED1_NODE, gpios),  // LED 1 → temperatura normal
    GPIO_DT_SPEC_GET(LED2_NODE, gpios),  // LED 2 → warning low
    GPIO_DT_SPEC_GET(LED3_NODE, gpios),  // LED 3 → warning high
};

void led_task(void *arg1, void *arg2, void *arg3)
{
    // Verifica se os dispositivos estão prontos
    for (int i = 0; i < 4; i++) {
        if (!device_is_ready(leds[i].port)) {
            printk("LED %d not ready\n", i);
            return;
        }
        gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
    }

    while (1) {
        k_mutex_lock(&RTDB_mutex, K_FOREVER);

        int on = RTDB.systemOn;
        int cur = RTDB.currentTemp;
        int tgt = RTDB.targetTemp;

        // LED 0 - estado do sistema
        gpio_pin_set_dt(&leds[0], on);

        if (!on) {
            // sistema desligado → apagar todos os outros LEDs
            for (int i = 1; i < 4; i++) {
                gpio_pin_set_dt(&leds[i], 0);
            }
        } else {
            // sistema ligado → avaliar temperatura
            if (cur >= tgt - 2 && cur <= tgt + 2) 
            {
                gpio_pin_set_dt(&leds[1], 1); // normal
                gpio_pin_set_dt(&leds[2], 0);
                gpio_pin_set_dt(&leds[3], 0);
            } 
            else if (cur < tgt - 2) 
            {
                gpio_pin_set_dt(&leds[1], 0);
                gpio_pin_set_dt(&leds[2], 1); // warning low
                gpio_pin_set_dt(&leds[3], 0);
            } 
            else 
            {
                gpio_pin_set_dt(&leds[1], 0);
                gpio_pin_set_dt(&leds[2], 0);
                gpio_pin_set_dt(&leds[3], 1); // warning high
            }
        }

        k_mutex_unlock(&RTDB_mutex);
        k_msleep(100);
    }
}