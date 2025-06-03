#include <zephyr/kernel.h>
#include "io.h"
#include "uart.h"
#include "rtdb.h"
#include "pwm.h"
#include "sensor_i2c.h"

#define TEMP_UPDATE_INTERVAL_MS 1000
#define CONTROL_INTERVAL_MS     500

bool setpoint_defined = false;
int temp_setpoint_buffer = 25;

K_MUTEX_DEFINE(buffer_mutex);

int get_temp_buffer(void) {
    int val;
    k_mutex_lock(&buffer_mutex, K_FOREVER);
    val = temp_setpoint_buffer;
    k_mutex_unlock(&buffer_mutex);
    return val;
}

void set_temp_buffer(int v) {
    k_mutex_lock(&buffer_mutex, K_FOREVER);
    temp_setpoint_buffer = v;
    k_mutex_unlock(&buffer_mutex);
}

void temperature_task(void)
{
    while (1) {
        if (!setpoint_defined) {
            k_msleep(500);
            continue;
        }

        int temp = sensor_init_read();
        if (temp >= 0) {
            rtdb_set_current_temp(temp);
        }
        k_msleep(TEMP_UPDATE_INTERVAL_MS);
    }
}

void control_task(void)
{
    int last_temp = -1000;
    int last_set = -1000;

    while (1) {
        if (!rtdb_get_system_state()) {
            io_set_led(0, 0);  // System off
            io_set_led(1, 0);
            io_set_led(2, 0);
            io_set_led(3, 0);
            last_temp = -1000;
            last_set = -1000;
            k_msleep(CONTROL_INTERVAL_MS);
            continue;
        }

        int t = rtdb_get_current_temp();
        int s = rtdb_get_set_temp();

        if (t != last_temp || s != last_set) {
            printk("\rCurrent Temperature: %2d ºC    |    Desired Temperature: %2d ºC\033[K", t, s);
            last_temp = t;
            last_set = s;
        }

        if (t < s - 2) {
            io_set_led(1, 0);
            io_blink_led(2);
            io_set_led(3, 0);
        } else if (t > s + 2) {
            io_set_led(1, 0);
            io_set_led(2, 0);
            io_blink_led(3);
        } else {
            io_set_led(1, 1);
            io_set_led(2, 0);
            io_set_led(3, 0);
        }

        k_msleep(CONTROL_INTERVAL_MS);
    }
}

K_THREAD_DEFINE(temp_id, 2048, temperature_task, NULL, NULL, NULL, 4, 0, 0);  // Prioridade + alta
K_THREAD_DEFINE(ctrl_id, 2048, control_task, NULL, NULL, NULL, 5, 0, 0); 

void main(void)
{
    printk("Boot start\n");
    io_init();
    uart_init();
    rtdb_init();

    if (pwm_ctrl_init() < 0) {
        printk("PWM init failed\n");
        return;
    }

    pwm_ctrl_set_duty_cycle_percent(50);  // Inicialmente 50% para teste

    printk("\033[2J\033[H");  // Clear screen and go home
printk("╔════════════════════════════════════╗\n");
printk("║        SMART HOME HEATER           ║\n");
printk("╚════════════════════════════════════╝\n\n");

printk("[SETUP] Please define desired temperature using buttons ▲ (Button 2) and ▼ (Button 4).\n");
printk("[INFO] Press Button 1 to confirm.\n\n");
printk("Desired Temperature: %2d ºC", temp_setpoint_buffer);
}