#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "io.h"
#include "rtdb.h"

#define BTN0_NODE DT_ALIAS(sw0)
#define BTN1_NODE DT_ALIAS(sw1)
#define BTN3_NODE DT_ALIAS(sw3)
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

static const struct gpio_dt_spec leds[] = {
    GPIO_DT_SPEC_GET(LED0_NODE, gpios),
    GPIO_DT_SPEC_GET(LED1_NODE, gpios),
    GPIO_DT_SPEC_GET(LED2_NODE, gpios),
    GPIO_DT_SPEC_GET(LED3_NODE, gpios),
};

static const struct gpio_dt_spec buttons[] = {
    GPIO_DT_SPEC_GET(BTN0_NODE, gpios),
    GPIO_DT_SPEC_GET(BTN1_NODE, gpios),
    GPIO_DT_SPEC_GET(BTN3_NODE, gpios),
};

extern bool setpoint_defined;
extern int temp_setpoint_buffer;

static struct gpio_callback btn_cb_data[3];

void io_set_led(int id, int state)
{
    gpio_pin_set_dt(&leds[id], state);
}

void io_blink_led(int id)
{
    static bool state = false;
    gpio_pin_set_dt(&leds[id], state);
    state = !state;
}

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    if (cb == &btn_cb_data[0]) {  // Botão 1 (Confirmar ou ligar sistema)
        gpio_pin_interrupt_configure_dt(&buttons[0], GPIO_INT_DISABLE);

        if (!setpoint_defined) {
            rtdb_set_set_temp(temp_setpoint_buffer);
            setpoint_defined = true;
            printk("\n[INFO] Desired temperature confirmed: %d ºC\n", temp_setpoint_buffer);
            printk("[INFO] System is now ready!\n");
            printk("[INFO] Press Button 1 again to turn ON/OFF the system.\n\n");
        } else {
            rtdb_toggle_system_state();
            io_set_led(0, rtdb_get_system_state());
        }

        gpio_pin_interrupt_configure_dt(&buttons[0], GPIO_INT_EDGE_TO_ACTIVE);
    }

    else if (cb == &btn_cb_data[1]) {  // Botão 2
        gpio_pin_interrupt_configure_dt(&buttons[1], GPIO_INT_DISABLE);

        if (!setpoint_defined) {
            int val = get_temp_buffer();
            set_temp_buffer(val + 1);
            printk("\rDesired Temperature: %2d ºC\033[K", get_temp_buffer());
        } else {
            rtdb_increment_set_temp();
            int s = rtdb_get_set_temp();
            int t = rtdb_get_current_temp();

            // Intercalar info e reprint
            printk("\n[INFO] Desired temperature set to: %d ºC\n", s);
            printk("Current Temperature: %2d ºC    |    Desired Temperature: %2d ºC\033[K", t, s);
        }

        gpio_pin_interrupt_configure_dt(&buttons[1], GPIO_INT_EDGE_TO_ACTIVE);
    }   

    else if (cb == &btn_cb_data[2]) {  // Botão 4
        gpio_pin_interrupt_configure_dt(&buttons[2], GPIO_INT_DISABLE);

        if (!setpoint_defined) {
            int val = get_temp_buffer();
            set_temp_buffer(val - 1);
            printk("\rDesired Temperature: %2d ºC\033[K", get_temp_buffer());
        } else {
            rtdb_decrement_set_temp();
            int s = rtdb_get_set_temp();
            int t = rtdb_get_current_temp();

            printk("\n[INFO] Desired temperature set to: %d ºC\n", s);
            printk("Current Temperature: %2d ºC    |    Desired Temperature: %2d ºC\033[K", t, s);
        }

        gpio_pin_interrupt_configure_dt(&buttons[2], GPIO_INT_EDGE_TO_ACTIVE);
    }
}

void io_init(void)
{
    printk("IO initialized\n");

    for (int i = 0; i < 4; i++) {
        if (!device_is_ready(leds[i].port)) return;
        gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
    }

    for (int i = 0; i < 3; i++) {
        if (!device_is_ready(buttons[i].port)) return;
        gpio_pin_configure_dt(&buttons[i], GPIO_INPUT);
        gpio_pin_interrupt_configure_dt(&buttons[i], GPIO_INT_EDGE_TO_ACTIVE);
        gpio_init_callback(&btn_cb_data[i], button_pressed, BIT(buttons[i].pin));
        gpio_add_callback(buttons[i].port, &btn_cb_data[i]);
    }
}