/*
 * exti.h
 *
 *  Created on: Mar 21, 2026
 *      Author: james
 */

#ifndef EXTI_H_
#define EXTI_H_

#include <stdint.h>

#define EXTI_DEBOUNCE_MS 	10U

void exti_init(void);

uint8_t exti_get_print_flag(void);
void exti_clear_print_flag(void );
#endif /* EXTI_H_ */
