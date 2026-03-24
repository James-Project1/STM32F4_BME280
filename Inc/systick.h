/*
 * systick.h
 *
 *  	Created on: Mar 11, 2026
 *      Author: james
 *
 *      systick_init() :
 *      	- configures systick to fire once every 1ms using processor
 *      	  clock. Must be called prior to use of API.
 *
 *     	delay_ms() :
 *     		- param[in] : ms	duration to block in milliseconds.
 *     		- blocking delay, cannot be called from an ISR context.
 *
 *     	systick_get_tick() :
 *     		- retval : uint32_t 	current tick count in ms.
 *     		- returns the raw ms tick counter, rolls over after ~49 days.
 *     		  used for debounce timing in exti.c.
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <stdint.h>

void systick_init(void);
void delay_ms(uint32_t ms);
uint32_t systick_get_tick(void);
uint8_t systick_get_measurement_flag(void);
void systick_clear_measurement_flag(void);

#endif /* SYSTICK_H_ */
