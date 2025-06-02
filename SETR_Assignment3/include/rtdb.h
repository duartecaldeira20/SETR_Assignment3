#ifndef RTDB_H
#define RTDB_H

void rtdb_init(void);

void rtdb_set_current_temp(int temp);
int rtdb_get_current_temp(void);

void rtdb_increment_current_temp(void);
void rtdb_decrement_current_temp(void);

int rtdb_get_set_temp(void);
void rtdb_increment_set_temp(void);
void rtdb_decrement_set_temp(void);

void rtdb_set_max_temp(int max);
int rtdb_get_system_state(void);
void rtdb_toggle_system_state(void);

#endif