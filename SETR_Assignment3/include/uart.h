#ifndef UART_H
#define UART_H

void uart_init(void);
void uart_send_string(const char *msg);
void uart_send_bytes(const uint8_t *buf, int len);

#endif