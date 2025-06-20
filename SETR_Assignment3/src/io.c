#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "io.h"
#include "rtdb.h"

#define BTN0_NODE DT_ALIAS(sw0)
#define BTN1_NODE DT_ALIAS(sw1)
#define BTN2_NODE DT_ALIAS(sw2)
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
    GPIO_DT_SPEC_GET(BTN2_NODE, gpios),
    GPIO_DT_SPEC_GET(BTN3_NODE, gpios),
};

extern bool setpoint_defined;
extern bool max_temp_defined;
extern int temp_setpoint_buffer;
extern int temp_max_buffer;
extern bool uart_mode;

static struct gpio_callback btn_cb_data[4];

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
    if (cb == &btn_cb_data[0]) 
    {  
        if (!setpoint_defined) 
        {
            setpoint_defined = true;
            printk("\n[INFO] Desired temperature set to: %d ºC\n", temp_setpoint_buffer);
            printk("[SETUP] Use ▲ (Button 2) and ▼ (Button 4) to define maximum temperature.\n");
        } 
        else if (!max_temp_defined) 
        {
            max_temp_defined = true;
            rtdb_set_set_temp(temp_setpoint_buffer);
            rtdb_set_max_temp(temp_max_buffer);
            printk("\n[INFO] Maximum temperature set to: %d ºC\n", temp_max_buffer);
            printk("[INFO] System is now ready!\n");
            printk("[INFO] Press Button 1 to toggle ON/OFF.\n\n");
        } 
        else 
        {
            rtdb_toggle_system_state();
            io_set_led(0, rtdb_get_system_state());
        }
    }

    else if (cb == &btn_cb_data[1]) 
    {  
        if (!setpoint_defined) 
        {
            temp_setpoint_buffer++;
        } 
        else if (!max_temp_defined) 
        {
            temp_max_buffer++;
        } 
        else 
        {
            rtdb_increment_set_temp();
        }

        if (!setpoint_defined) 
        {
            printk("\r[SETUP] Desired Temperature: %2d ºC   |   Maximum Temperature: ?? ºC\033[K", temp_setpoint_buffer);
        } 
        else if (!max_temp_defined) 
        {
            printk("\r[SETUP] Desired Temperature: %2d ºC   |   Maximum Temperature: %2d ºC\033[K", temp_setpoint_buffer, temp_max_buffer);
        } 
        else 
        {
            int t = rtdb_get_current_temp();
            int s = rtdb_get_set_temp();
            int m = rtdb_get_max_temp();
            printk("\n[INFO] Desired temperature set to: %d ºC\n", s);
            printk("Current Temperature: %2d ºC    |    Desired Temperature: %2d ºC    |    Maximum Temperature: %2d ºC\033[K", t, s, m);
        }
    }

    else if (cb == &btn_cb_data[3]) 
    {  
        if (!setpoint_defined) 
        {
            temp_setpoint_buffer--;
        } 
        else if (!max_temp_defined) 
        {
            temp_max_buffer--;
        } 
        else 
        {
            rtdb_decrement_set_temp();
        }

        if (!setpoint_defined) 
        {
            printk("\r[SETUP] Desired Temperature: %2d ºC   |   Maximum Temperature: ?? ºC\033[K", temp_setpoint_buffer);
        } 
        else if (!max_temp_defined) 
        {
            printk("\r[SETUP] Desired Temperature: %2d ºC   |   Maximum Temperature: %2d ºC\033[K", temp_setpoint_buffer, temp_max_buffer);
        } 
        else 
        {
            int t = rtdb_get_current_temp();
            int s = rtdb_get_set_temp();
            int m = rtdb_get_max_temp();
            printk("\n[INFO] Desired temperature set to: %d ºC\n", s);
            printk("Current Temperature: %2d ºC    |    Desired Temperature: %2d ºC    |    Maximum Temperature: %2d ºC\033[K", t, s, m);
        }
    }

    else if (cb == &btn_cb_data[2]) 
    {  
        if (!setpoint_defined || !max_temp_defined) return;

        uart_mode = !uart_mode;

        if (uart_mode) 
        {
            printk("\n\n[UART MODE] ─ UART Command Interface ─────────────\n");
            printk("Command: ");
        } 
        else 
        {
            int t = rtdb_get_current_temp();
            int s = rtdb_get_set_temp();
            int m = rtdb_get_max_temp();
            printk("\n[INFO] UART mode closed. Returning to system view...\n");
            printk("Current Temperature: %2d ºC    |    Desired Temperature: %2d ºC    |    Maximum Temperature: %2d ºC\033[K", t, s, m);
        }
    }
}

void io_init(void)
{
    for (int i = 0; i < 4; i++) 
    {
        if (!device_is_ready(leds[i].port)) return;
        gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
    }

    for (int i = 0; i < 4; i++) 
    {
        if (!device_is_ready(buttons[i].port)) return;
        gpio_pin_configure_dt(&buttons[i], GPIO_INPUT);
        gpio_pin_interrupt_configure_dt(&buttons[i], GPIO_INT_EDGE_TO_ACTIVE);
        gpio_init_callback(&btn_cb_data[i], button_pressed, BIT(buttons[i].pin));
        gpio_add_callback(buttons[i].port, &btn_cb_data[i]);
    }
}