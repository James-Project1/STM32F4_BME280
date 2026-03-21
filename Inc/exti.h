/*
 * exti.h
 *
 *  	Created on: Mar 21, 2026
 *      Author: james
 *
 *      exti_init() :
 *      	- configures PC13 and routes to EXTI line 13. Enables falling
 *      	  edge trigger. Configures NVIC.
 *
 *     	exti_get_print_flag() :
 *     		- returns current state of print flag.
 *
 *     	exti_clear_print_flag() :
 *     		-  clears print flag, resets to 0. Used by caller after the flag
 *     		   has been acted upon.
 *
 *     	Note : Debounce interval and NVIC priority are controlled macros in
 *     		   config.h.
 */

#ifndef EXTI_H_
#define EXTI_H_

#include <stdint.h>

void exti_init(void);
uint8_t exti_get_print_flag(void);
void exti_clear_print_flag(void);

#endif /* EXTI_H_ */
