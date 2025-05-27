#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "rtdb.h"
#include "buttons.h"
#include "led.h"
#include "uart.h"

#define STACK_SIZE 1024
#define PRIORITY 5

// Definição das stacks e dados das threads
K_THREAD_STACK_DEFINE(buttons_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(led_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(uart_stack, STACK_SIZE);

struct k_thread buttons_thread_data;
struct k_thread led_thread_data;
struct k_thread uart_thread_data;

void main(void)
{
    printk(">> Sistema iniciado (main)\n");

    init_rtdb(); // Inicializa a base de dados e mutex

    // Thread dos botões
    k_thread_create(&buttons_thread_data, buttons_stack,
        K_THREAD_STACK_SIZEOF(buttons_stack),
        buttons_task, NULL, NULL, NULL,
        PRIORITY, 0, K_NO_WAIT);

    // Thread dos LEDs
    k_thread_create(&led_thread_data, led_stack,
        K_THREAD_STACK_SIZEOF(led_stack),
        led_task, NULL, NULL, NULL,
        PRIORITY, 0, K_NO_WAIT);

    // Thread UART
    k_thread_create(&uart_thread_data, uart_stack,
        K_THREAD_STACK_SIZEOF(uart_stack),
        uart_task, NULL, NULL, NULL,
        PRIORITY, 0, K_NO_WAIT);
}