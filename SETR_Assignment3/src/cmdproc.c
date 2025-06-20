#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmdproc.h"
#include "rtdb.h"
#include "uart.h"

static void send_response(const char *cmd, const char *data)
{
    char frame[32];
    int sum = 0;

    sum += cmd[0];
    for (int i = 0; data[i] != '\0'; i++) sum += data[i];

    char cs[4];
    snprintf(cs, sizeof(cs), "%03d", sum % 256);
    snprintf(frame, sizeof(frame), "#%s%s%s!", cmd, data, cs);
    uart_send_string(frame);
}

static int checksum_valid(const uint8_t *buf, int len)
{
    int sum = 0;

    for (int i = 1; i < len - 4; i++)
    {
        sum += buf[i];
    }

    char received_cs[4] = { buf[len - 4], buf[len - 3], buf[len - 2], '\0' };
    char expected_cs[4];

    snprintf(expected_cs, sizeof(expected_cs), "%03d", sum % 256);

    return strncmp(received_cs, expected_cs, 3) == 0;
}

int cmd_process(const uint8_t *buf, int len)
{
    if (len < 6 || buf[0] != '#' || buf[len - 1] != '!') 
    {
        send_response("Ef", "");
        return -1;
    }

    if (!checksum_valid(buf, len)) 
    {
        send_response("Es", "");
        return -2;
    }

    char cmd = buf[1];

    switch (cmd) 
    {
        case 'C': 
        {
            printk("\nUART Response: ");
            int temp = rtdb_get_current_temp();
            char tstr[6];
            snprintf(tstr, sizeof(tstr), "%+03d", temp);
            send_response("c", tstr);

            
            break;
        }

        case 'M': 
        {
            printk("\nUART Response: ");

            if (len < 9) {
                send_response("Ei", "");
                return -3;
            }
            char tstr[4] = { buf[2], buf[3], buf[4], '\0' };
            int new_max = atoi(tstr);
            rtdb_set_max_temp(new_max);
            send_response("Eo", "");

            
            break;
        }

        case 'S': 
        {
            printk("\nUART Response: ");

            if (len < 11) {
            send_response("Ef", "");  // framing error (comando demasiado curto)
            return -3;
        }

        char kp_str[4] = { buf[2], buf[3], buf[4], '\0' };
        char ki_str[4] = { buf[5], buf[6], buf[7], '\0' };
        char kd_str[4] = { buf[8], buf[9], buf[10], '\0' };

        float kp = atoi(kp_str) / 100.0f;
        float ki = atoi(ki_str) / 100.0f;
        float kd = atoi(kd_str) / 100.0f;

        rtdb_set_pid_params(kp, ki, kd);  // assumes you’ve defined this function

        send_response("Eo", "");  // comando aceite

        printk("\nCommand: ");
        break;
        }   

        default:

            printk("\nUART Response: ");
            send_response("Ei", "");

            
            break;
    }

    return 0;
}