#ifndef IO_H
#define IO_H

void io_init(void);
void io_set_led(int id, int state);
void io_blink_led(int id);

#endif