#include <zephyr/kernel.h>
#include "io.h"
#include "uart.h"
#include "rtdb.h"
#include "pwm.h"
#include "sensor_i2c.h"

// === Variáveis globais do sistema ===
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

    float Kp = 2.0;
    float Ki = 0.1;
    float Kd = 1.0;

    while (1) {
        if (!rtdb_get_system_state()) {
            // Sistema desligado, LEDs e PWM a 0
            io_set_led(0, 0);
            io_set_led(1, 0);
            io_set_led(2, 0);
            io_set_led(3, 0);
            pwm_ctrl_set_duty_cycle_percent(0);
            last_temp = -1000;
            last_set = -1000;
            integral = 0;
            previous_error = 0;
            k_msleep(CONTROL_INTERVAL_MS);
            continue;
        }

        int t = rtdb_get_current_temp();
        int s = rtdb_get_set_temp();

        // Atualizar linha do terminal se algo mudou
        if (t != last_temp || s != last_set) {
            printk("\rCurrent Temperature: %2d ºC    |    Desired Temperature: %2d ºC\033[K", t, s);
            last_temp = t;
            last_set = s;
        }

        // PID calculation
        float error = (float)(s - t);
        integral += error * (CONTROL_INTERVAL_MS / 1000.0f);
        float derivative = (error - previous_error) / (CONTROL_INTERVAL_MS / 1000.0f);
        float output = Kp * error + Ki * integral + Kd * derivative;
        previous_error = error;

        // Clamp output
        if (output < 0) output = 0;
        if (output > 100) output = 100;

        pwm_ctrl_set_duty_cycle_percent(100 - (int)output);

        // LED logic (podes ajustar isto consoante preferências visuais)
        if (error > 2) {
            io_set_led(1, 0);
            io_blink_led(2); // heating
            io_set_led(3, 0);
        } else if (error < -2) {
            io_set_led(1, 0);
            io_set_led(2, 0);
            io_blink_led(3); // cooling
        } else {
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

    io_init();            // LEDs, botões
    uart_init();          // Isto agora imprime antes de linhas com \r
    rtdb_init();          // RTDB

    if (pwm_ctrl_init() < 0) {
        printk("PWM init failed\n");
        return;
    }

    pwm_ctrl_set_duty_cycle_percent(50);  // Para já 50%

    // Limpar e desenhar cabeçalho
    printk("\033[2J\033[H");
    printk("╔════════════════════════════════════╗\n");
    printk("║        SMART HOME HEATER           ║\n");
    printk("╚════════════════════════════════════╝\n\n");

    printk("[SETUP] Use ▲ (Button 2) and ▼ (Button 4) to define setpoint.\n");
    printk("[INFO] Press Button 1 to confirm setpoint.\n\n");

    // Imprime a primeira linha de setpoint com limpeza da linha
    printk("\r\033[2K[SETUP] Desired Temperature: %2d ºC  |  Maximum Temperature: ?? ºC", temp_setpoint_buffer);
}