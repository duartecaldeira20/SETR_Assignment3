/**
 * @file rtdb.h
 * @brief Real-Time Database.
 *
 * This module stores system state including current temperature,
 * desired setpoint, maximum limit, and PID parameters.
 * Thread-safe access is ensured via mutexes.
 *
 * @authors Duarte Caldeira, Tiago Saraiva
 * 
 */

#ifndef RTDB_H
#define RTDB_H

/**
 * @brief Initializes the RTDB module.
 */
void rtdb_init(void);

/**
 * @brief Sets the current measured temperature.
 *
 * @param temp The current temperature in degrees Celsius.
 */
void rtdb_set_current_temp(int temp);

/**
 * @brief Gets the current measured temperature.
 *
 * @return Current temperature in degrees Celsius.
 */
int rtdb_get_current_temp(void);

/**
 * @brief Sets the desired setpoint temperature.
 *
 * @param temp The desired temperature in degrees Celsius.
 */
void rtdb_set_set_temp(int temp);

/**
 * @brief Gets the desired setpoint temperature.
 *
 * @return Desired temperature in degrees Celsius.
 */
int rtdb_get_set_temp(void);

/**
 * @brief Increments the desired setpoint temperature.
 */
void rtdb_increment_set_temp(void);

/**
 * @brief Decrements the desired setpoint temperature.
 */
void rtdb_decrement_set_temp(void);

/**
 * @brief Sets the maximum allowed temperature.
 *
 * @param max Maximum temperature in degrees Celsius.
 */
void rtdb_set_max_temp(int max);

/**
 * @brief Gets the maximum allowed temperature.
 *
 * @return Maximum temperature in degrees Celsius.
 */
int rtdb_get_max_temp(void);

/**
 * @brief Toggles the current system ON/OFF state.
 */
void rtdb_toggle_system_state(void);

/**
 * @brief Gets the current system ON/OFF state.
 *
 * @return 1 if ON, 0 if OFF.
 */
int rtdb_get_system_state(void);

/**
 * @brief Sets the PID control parameters.
 *
 * @param kp Proportional gain.
 * @param ki Integral gain.
 * @param kd Derivative gain.
 */
void rtdb_set_pid_params(float kp, float ki, float kd);

/**
 * @brief Gets the PID control parameters.
 *
 * @param kp Pointer to store proportional gain.
 * @param ki Pointer to store integral gain.
 * @param kd Pointer to store derivative gain.
 */
void rtdb_get_pid_params(float *kp, float *ki, float *kd);

#endif // RTDB_H