/*
 * systick.h
 *
 *  Created on: Mar 11, 2026
 *      Author: james
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <stdint.h>

void systick_init(void);
void delay_ms(uint32_t ms);

#endif /* SYSTICK_H_ */
