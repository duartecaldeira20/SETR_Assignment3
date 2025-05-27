#ifndef RTDB_H
#define RTDB_H

#include <zephyr/kernel.h>

struct rtdb_t {
    bool systemOn;         // Estado do sistema (on/off)
    int currentTemp;       // Temperatura atual
    int targetTemp;        // Temperatura alvo
};

extern struct rtdb_t RTDB;
extern struct k_mutex RTDB_mutex;

void init_rtdb(void);

#endif // RTDB_H