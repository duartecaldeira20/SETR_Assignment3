#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <string.h>
#include <stdio.h>
#include "cmdproc.h"
#include "uart.h"

#define UART_DEV_NODE DT_NODELABEL(uart0)

static const struct device *uart_dev = DEVICE_DT_GET(UART_DEV_NODE);

static uint8_t rx_buf[UART_BUF_SIZE];
static uint8_t rx_len = 0;

static uint8_t tx_buf[UART_TX_SIZE];
static uint8_t tx_len = 0;

void uart_reset_tx_buffer(void) {
    memset(tx_buf, 0, UART_TX_SIZE);
    tx_len = 0;
}

void uart_get_tx_buffer(uint8_t *buf, int *len) {
    *len = tx_len;
    memcpy(buf, tx_buf, tx_len);
}

static void uart_send_reply(void) {
    for (int i = 0; i < tx_len; i++) {
        uart_poll_out(uart_dev, tx_buf[i]);
    }
    uart_reset_tx_buffer();
}

void uart_receive_byte(uint8_t byte) {
    if (rx_len < UART_BUF_SIZE) {
        rx_buf[rx_len++] = byte;
        if (byte == EOF_SYM) {
            // Copy received bytes into cmdproc buffer
            resetRxBuffer();
            for (int i = 0; i < rx_len; i++) {
                rxChar(rx_buf[i]);
            }

            // Process command
            int res = cmdProcessor();

            // Get response and send
            getTxBuffer(tx_buf, (int *)&tx_len);
            uart_send_reply();

            // Reset for next command
            rx_len = 0;
        }
    } else {
        rx_len = 0; // overflow protection
    }
}

void uart_process_rx(void) {
    while (uart_irq_update(uart_dev) && uart_irq_is_pending(uart_dev)) {
        if (uart_irq_rx_ready(uart_dev)) {
            uint8_t byte;
            int r = uart_fifo_read(uart_dev, &byte, 1);
            if (r > 0) {
                uart_receive_byte(byte);
            }
        }
    }
}

void uart_init(void) {
    if (!device_is_ready(uart_dev)) {
        printk("UART device not ready\n");
        return;
    }

    uart_irq_rx_enable(uart_dev);
}