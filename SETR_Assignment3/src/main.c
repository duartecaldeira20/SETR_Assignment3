#include <zephyr/kernel.h>
#include "io.h"
#include "uart.h"
#include "rtdb.h"
#include "pwm.h"
#include "sensor_i2c.h"

bool setpoint_defined = false;
bool max_temp_defined = false;
bool uart_mode = false;

int temp_setpoint_buffer = 25;
int temp_max_buffer = 40;

#define TEMP_UPDATE_INTERVAL_MS 1000
#define CONTROL_INTERVAL_MS     500

void temperature_task(void)
{
    while (1) {
        if (!setpoint_defined || !max_temp_defined || !rtdb_get_system_state()) {
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
    float integral = 0.0f;
    float previous_error = 0.0f;

    float Kp, Ki, Kd;
    rtdb_get_pid_params(&Kp, &Ki, &Kd);

    while (1) 
    {
        if (!rtdb_get_system_state()) 
        {
            
            io_set_led(0, 0);
            io_set_led(1, 0);
            io_set_led(2, 0);
            io_set_led(3, 0);
            pwm_set_dc(0);
            last_temp = -1000;
            last_set = -1000;
            integral = 0;
            previous_error = 0;
            k_msleep(CONTROL_INTERVAL_MS);
            continue;
        }

        int t = rtdb_get_current_temp();
        int s = rtdb_get_set_temp();
        int m = rtdb_get_max_temp();

        
        if ((t != last_temp || s != last_set) && !uart_mode) 
        {
            int m = rtdb_get_max_temp();
            printk("\rCurrent Temperature: %2d ºC    |    Desired Temperature: %2d ºC    |    Maximum Temperature: %2d ºC\033[K", t, s, m);
            last_temp = t;
            last_set = s;
        }

        //if (t >= m) {
        //    pwm_ctrl_set_duty_cycle_percent(0);
        //    k_msleep(CONTROL_INTERVAL_MS);
        //    continue;
        //}

        float error = (float)(s - t);
        integral += error * (CONTROL_INTERVAL_MS / 1000.0f);
        float derivative = (error - previous_error) / (CONTROL_INTERVAL_MS / 1000.0f);
        float output = Kp * error + Ki * integral + Kd * derivative;

        if (output < 0) output = 0;
        if (output > 100) output = 100;

        pwm_set_dc(100 - (int)output);

        if (error > 2) 
        {
            io_set_led(1, 0);
            io_blink_led(2); // heating
            io_set_led(3, 0);
        } 
        else if (error < -2) 
        {
            io_set_led(1, 0);
            io_set_led(2, 0);
            io_blink_led(3); // cooling
        } 
        else 
        {
            io_set_led(1, 1); // stable
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
    pwm_init();

    printk("\033[2J\033[H");
    printk("╔════════════════════════════════════╗\n");
    printk("║        SMART HOME HEATER           ║\n");
    printk("╚════════════════════════════════════╝\n\n");

    printk("[SETUP] Use ▲ (Button 2) and ▼ (Button 4) to define setpoint.\n");
    printk("[INFO] Press Button 1 to confirm setpoint.\n\n");
    printk("\r\033[2K[SETUP] Desired Temperature: %2d ºC  |  Maximum Temperature: ?? ºC", temp_setpoint_buffer);
}