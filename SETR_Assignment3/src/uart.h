#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

#include <zephyr/kernel.h>

#define UART_BUF_SIZE 64

void uart_init(void);
void uart_process_rx(void);
void uart_receive_byte(uint8_t byte);
void uart_get_tx_buffer(uint8_t *buf, int *len);
void uart_reset_tx_buffer(void);

#endif // UART_INTERFACE_H