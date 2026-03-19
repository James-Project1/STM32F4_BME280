/*
 * systick.c
 *
 *  Created on: Mar 19, 2026
 *      Author: james
 */
#include "config.h"
#include "systick.h"
#include "stm32f401xe.h"

#define SYSTICK_LOAD_VAL    ((SYSCLK_HZ / 1000U) - 1U)
static volatile uint32_t tick = 0U;

void systick_init(void)
{
    SysTick->LOAD = SYSTICK_LOAD_VAL;
    SysTick->VAL  = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |   // processor clock
                    SysTick_CTRL_TICKINT_Msk    |   // enable interrupt
                    SysTick_CTRL_ENABLE_Msk;        // start counter
}

void SysTick_Handler(void)
{
    tick++;
}

void delay_ms(uint32_t ms)
{
    uint32_t start = tick;
    while((tick - start) < ms);
}

