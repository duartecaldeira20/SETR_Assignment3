/**
 * @file cmdproc.h
 * @brief Command processor for UART protocol handling.
 *
 * This module parses incoming UART commands, validates checksum and framing,
 * and updates the system state accordingly.
 *
 * @authors Duarte Caldeira, Tiago Saraiva
 *
 */

#ifndef CMDPROC_H
#define CMDPROC_H

/**
 * @brief Processes a command received via UART.
 *
 *
 * @param buf Pointer to the received data buffer.
 * @param len Length of the received data.
 * @return 0 if successful, negative value if there's an error.
 */
int cmd_process(const uint8_t *buf, int len);

#endif