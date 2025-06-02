#include <zephyr/kernel.h>
#include "io.h"
#include "uart.h"
#include "rtdb.h"
#include "emul.h"

#define TEMP_UPDATE_INTERVAL_MS 1000
#define CONTROL_INTERVAL_MS     500

void temperature_task(void)
{
    while (1) {
        int temp = rtdb_get_current_temp();  // Temperatura simulada fixa a 25
        rtdb_set_current_temp(temp);
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
            printk("Current Temp: %d°C | Set Temp: %d°C\n", t, s);
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

K_THREAD_DEFINE(temp_id, 1024, temperature_task, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(ctrl_id, 1024, control_task, NULL, NULL, NULL, 5, 0, 0);

void main(void)
{
    printk("Boot start\n");
    io_init();
    uart_init();
    rtdb_init();
    emul_init();
    printk("System ready\n");
}