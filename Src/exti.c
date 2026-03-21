/*
 * exti.c
 *
 *  Created on: Mar 21, 2026
 *      Author: james
 */
#include "exti.h"
#include "stm32f401xe.h"
#include "config.h"
#include "systick.h"
/* private API declarations */
static void exti_clock_init(void);
static void exti_gpio_init(void);
static void exti_interrupt_init(void);
/* static variables */
static volatile uint8_t print_flag = 0U;
static volatile uint32_t last_press_tick = 0U;
/* public API definitions */
void exti_init(void) {
	exti_clock_init();
	exti_gpio_init();
	exti_interrupt_init();
}

uint8_t exti_get_print_flag(void) {
	return print_flag;
}

void exti_clear_print_flag(void) {
	print_flag = 0U;
}

void EXTI15_10_IRQHandler(void) {
	if(EXTI->PR & (1U << 13U)) {
		EXTI->PR |= (1U << 13U); // clears
		uint32_t curr_tick = systick_get_tick();
		if((curr_tick - last_press_tick) < EXTI_DEBOUNCE_MS) return;
		last_press_tick = curr_tick;
		print_flag = 1U; // set a print flag
	}
}

/* private API definitions */
static void exti_clock_init(void) {
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	(void)RCC->APB2ENR;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	(void)RCC->AHB1ENR;
}

static void exti_gpio_init(void) {
	GPIOC->MODER &= ~(3U << 26U); // set as input
}

static void exti_interrupt_init(void) {
	SYSCFG->EXTICR[3] &= ~(0xFU << 4U);
	SYSCFG->EXTICR[3] |=  (0x2U << 4U);    // set port C

	EXTI->FTSR |= (1U << 13U);    // falling edge on line 13
	EXTI->IMR |= (1U << 13U);    // unmask line 13

	NVIC_SetPriority(EXTI15_10_IRQn, EXTI_PRIORITY); // init NVIC
	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}
