/**
 * @file uart.h
 * @brief UART communication interface for Smart Home Heater.
 *
 * This module enables UART communication with external devices.
 * It supports sending commands and receiving frames using UART0.
 *
 * @authors Duarte Caldeira, Tiago Saraiva
 * 
 */

#ifndef UART_H
#define UART_H

/**
 * @brief Initializes UART and sets up RX callback.
 */
void uart_init(void);

/**
 * @brief Sends a null-terminated string over UART.
 *
 * @param msg Pointer to the string to send.
 */
void uart_send_string(const char *msg);

/**
 * @brief Sends bytes over UART.
 *
 * @param buf Pointer to the buffer.
 * @param len Number of bytes to send.
 */
void uart_send_bytes(const uint8_t *buf, int len);

#endif // UART_H