#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include "rtdb.h"

// Definições dos botões com base nos aliases do DTS
#define BTN0_NODE DT_ALIAS(sw0)
#define BTN1_NODE DT_ALIAS(sw1)
#define BTN2_NODE DT_ALIAS(sw2)
#define BTN3_NODE DT_ALIAS(sw3)

static const struct gpio_dt_spec btns[] = {
    GPIO_DT_SPEC_GET(BTN0_NODE, gpios),
    GPIO_DT_SPEC_GET(BTN1_NODE, gpios),
    GPIO_DT_SPEC_GET(BTN2_NODE, gpios),
    GPIO_DT_SPEC_GET(BTN3_NODE, gpios),
};

static int last_state[4] = {1, 1, 1, 1}; // Estado anterior de cada botão (ativo-alto por defeito)

void buttons_task(void *arg1, void *arg2, void *arg3)
{
    for (int i = 0; i < 4; i++) {
        if (!device_is_ready(btns[i].port)) {
            return;
        }
        gpio_pin_configure_dt(&btns[i], GPIO_INPUT);
    }

    while (1) 
    {
        for (int i = 0; i < 4; i++) {
            int state = gpio_pin_get_dt(&btns[i]);
            if (state == 0 && last_state[i] == 1) { // Detetar transição de 1 → 0 (pressionado)
                k_mutex_lock(&RTDB_mutex, K_FOREVER);
                switch (i) {
                    case 0:
                        RTDB.systemOn = !RTDB.systemOn;
                        break;
                    case 1:
                        RTDB.currentTemp++;
                        break;
                    case 2:
                        RTDB.currentTemp--;
                        break;
                    case 3:
                        // ação adicional opcional
                        break;
                }
                k_mutex_unlock(&RTDB_mutex);
            }
            last_state[i] = state;
        }

        k_msleep(50); // Debounce simples
    }
}