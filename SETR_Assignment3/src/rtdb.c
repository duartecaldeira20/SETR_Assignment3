#include "rtdb.h"

struct rtdb_t RTDB = {
    .systemOn = false,
    .currentTemp = 20,      // Temperatura inicial fictícia
    .targetTemp = 25        // Temperatura alvo por defeito
};

struct k_mutex RTDB_mutex;

void init_rtdb(void) {
    k_mutex_init(&RTDB_mutex);
}