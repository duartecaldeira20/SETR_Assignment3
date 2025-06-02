#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <string.h>
#include "uart.h"
#include "cmdproc.h"

#define UART_NODE       DT_NODELABEL(uart0)
#define RXBUF_SIZE      64
#define RX_TIMEOUT_US   1000

const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);
static uint8_t rx_buf[RXBUF_SIZE];
static uint8_t rx_data[RXBUF_SIZE];
static volatile int rx_len = 0;

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    switch (evt->type) {

    case UART_RX_RDY: {
        for (int i = 0; i < evt->data.rx.len; i++) {
            char c = rx_buf[evt->data.rx.offset + i];

            if (c == '#') {
                rx_len = 0;  // início da frame
            }

            if (rx_len < RXBUF_SIZE - 1) {
                rx_data[rx_len++] = c;
            }

            if (c == '!') {
                rx_data[rx_len] = '\0';  // termina string
                printk("Received frame: %s\n", rx_data);
                cmd_process(rx_data, rx_len);
                rx_len = 0;
                memset(rx_data, 0, sizeof(rx_data));
            }
        }
        break;
    }

    case UART_RX_DISABLED:
        uart_rx_enable(uart_dev, rx_buf, sizeof(rx_buf), RX_TIMEOUT_US);
        break;

    default:
        break;
    }
}

void uart_init(void)
{
    if (!device_is_ready(uart_dev)) {
        printk("UART device not ready\n");
        return;
    }

    if (uart_callback_set(uart_dev, uart_cb, NULL) != 0) {
        printk("Failed to set UART callback\n");
        return;
    }

    if (uart_rx_enable(uart_dev, rx_buf, sizeof(rx_buf), RX_TIMEOUT_US) != 0) {
        printk("Failed to enable UART RX\n");
        return;
    }

    printk("UART initialized\n");
}

void uart_send_string(const char *msg)
{
    uart_tx(uart_dev, msg, strlen(msg), SYS_FOREVER_MS);
}

void uart_send_bytes(const uint8_t *buf, int len)
{
    uart_tx(uart_dev, buf, len, SYS_FOREVER_MS);
}