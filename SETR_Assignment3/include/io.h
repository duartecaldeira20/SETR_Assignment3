/**
 * @file io.h
 * @brief GPIO interface for LED control and button handling in the Smart Home Heater project.
 *
 * This module provides functions for initializing and controlling
 * GPIO-connected LEDs and buttons. It supports system interaction
 * via physical buttons and visual status indication through LEDs.
 *
 * @authors Duarte Caldeira, Tiago Saraiva
 * 
 */

#ifndef IO_H
#define IO_H

/**
 * @brief Initializes all LED and button GPIOs.
 *
 * Configures each LED as output and each button as input with interrupt
 * capabilities. Also registers callback functions for button events.
 */
void io_init(void);

/**
 * @brief Sets the state of a specific LED.
 *
 * @param id The ID/index of the LED (0 to 3).
 * @param state The desired state: 1 (on) or 0 (off).
 */
void io_set_led(int id, int state);

/**
 * @brief Toggles the state of a specific LED.
 *
 * Useful for blinking indication.
 *
 * @param id The ID/index of the LED (0 to 3).
 */
void io_blink_led(int id);

#endif // IO_H