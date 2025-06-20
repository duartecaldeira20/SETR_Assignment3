#include <zephyr/kernel.h>
#include "rtdb.h"

static int current_temp = 0;
static int set_temp = 25;
static int max_temp = 50;
static bool system_on = false;

static float Kp = 2.0f;
static float Ki = 0.1f;
static float Kd = 1.0f;

K_MUTEX_DEFINE(rtdb_mutex);

void rtdb_init(void) {}

void rtdb_set_current_temp(int temp)
{
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    current_temp = temp;
    k_mutex_unlock(&rtdb_mutex);
}

int rtdb_get_current_temp(void)
{
    int temp;
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    temp = current_temp;
    k_mutex_unlock(&rtdb_mutex);
    return temp;
}

//void rtdb_increment_current_temp(void)
//{
//    k_mutex_lock(&rtdb_mutex, K_FOREVER);
//    if (current_temp < max_temp) current_temp++;
//    k_mutex_unlock(&rtdb_mutex);
//}

//void rtdb_decrement_current_temp(void)
//{
//    k_mutex_lock(&rtdb_mutex, K_FOREVER);
//    if (current_temp > 0) current_temp--;
//    k_mutex_unlock(&rtdb_mutex);
//}

int rtdb_get_set_temp(void)
{
    int temp;
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    temp = set_temp;
    k_mutex_unlock(&rtdb_mutex);
    return temp;
}

void rtdb_increment_set_temp(void)
{
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    if (set_temp < max_temp) set_temp++;
    k_mutex_unlock(&rtdb_mutex);
}

void rtdb_decrement_set_temp(void)
{
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    if (set_temp > 0) set_temp--;
    k_mutex_unlock(&rtdb_mutex);
}

void rtdb_set_set_temp(int temp)
{
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    set_temp = temp;
    k_mutex_unlock(&rtdb_mutex);
}

void rtdb_set_max_temp(int max)
{
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    max_temp = max;
    k_mutex_unlock(&rtdb_mutex);
}

int rtdb_get_max_temp(void)
{
    int max;
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    max = max_temp;
    k_mutex_unlock(&rtdb_mutex);
    return max;
}

int rtdb_get_system_state(void)
{
    int state;
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    state = system_on;
    k_mutex_unlock(&rtdb_mutex);
    return state;
}

void rtdb_toggle_system_state(void)
{
    k_mutex_lock(&rtdb_mutex, K_FOREVER);
    system_on = !system_on;
    k_mutex_unlock(&rtdb_mutex);
}

void rtdb_set_pid_params(float p, float i, float d) {
    Kp = p;
    Ki = i;
    Kd = d;
}

void rtdb_get_pid_params(float *p, float *i, float *d) {
    if (p) *p = Kp;
    if (i) *i = Ki;
    if (d) *d = Kd;
}