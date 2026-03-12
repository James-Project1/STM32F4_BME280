/*
 * i2c.c
 *
 *  Created on: Mar 11, 2026
 *      Author: james
 */

/* includes */
#include "i2c.h"
#include "stm32f401xe.h"
#include "config.h"

/* private API */
static void i2c1_clock_init(void);
static void i2c1_gpio_init(void);
static void i2c1_configure(void);
static void i2c1_irq_init(void);
/* I2C IRQ Handler */
void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);
/* static variables */
static volatile i2c_context_t handle;
/* public API definitions */
void i2c1_init(void) {
	i2c1_clock_init();
	i2c1_gpio_init();
	i2c1_configure();
	i2c1_irq_init();
}


/* I2C IRQ Handler definitions */

/* private API definitions */
static void i2c1_clock_init(void) {
	// enable GPIOB peripheral clock
	RCC->AHB1ENR |= (1U << 1U);
	(void)RCC->AHB1ENR;
	// enable i2c1 peripheral clock
	RCC->APB1ENR |= (1U << 21U);
	(void)RCC->APB1ENR;
	// reset i2c1 to know state
	RCC->APB1RSTR |= (1U << 21U);
	RCC->APB1RSTR &= ~(1U << 21U);
}

static void i2c1_gpio_init(void) {
	/* configure PB8 for i2C1 */
	GPIOB->MODER &= ~(3U << 16U);
	GPIOB->MODER |= (2U << 16U); // AF
	GPIOB->AFR[1] &= ~(15U << 0U);
	GPIOB->AFR[1] |= (4U << 0U); // AF4

	GPIOB->OTYPER |= (1U << 8U); // open Drain

	GPIOB->PUPDR &= ~(3U << 16U);
	GPIOB->PUPDR |= (1U << 16U); // pull up

	GPIOB->OSPEEDR &= ~(3U << 16U);
	GPIOB->OSPEEDR |=  (3U << 16U);   // high speed

	/* configure PB9 for i2c1 */
	GPIOB->MODER &= ~(3U << 18U);
	GPIOB->MODER |=  (2U << 18U);   // AF
	GPIOB->AFR[1] &= ~(15U << 4U);
	GPIOB->AFR[1] |=  (4U  << 4U);  // AF4 (I2C1)

	GPIOB->OTYPER |=  (1U << 9U);   // open-drain

	GPIOB->PUPDR &= ~(3U << 18U);
	GPIOB->PUPDR |=  (1U << 18U);   // pull-up

	GPIOB->OSPEEDR &= ~(3U << 18U);
	GPIOB->OSPEEDR |=  (3U << 18U); // high speed
}

static void i2c1_configure(void) {
	/* configure i2c1 peripheral */
	I2C1->CR1 &= ~I2C_CR1_PE; // disable i2c1

	I2C1->CR2 &= ~I2C_CR2_FREQ;
	I2C1->CR2 |= (APB1_CLK_MHZ & I2C_CR2_FREQ); // peripheral clock frequency

	I2C1->CCR &= ~I2C_CCR_FS; // standard mode
	I2C1->CCR &= ~I2C_CCR_CCR;

	uint32_t ccr = (APB1_CLK_HZ / (2U * I2C1_SPEED_HZ));
	if(ccr < 4U)ccr = 4U;
	I2C1->CCR |= (ccr & I2C_CCR_CCR); // set i2c speed

	I2C1->TRISE &= ~I2C_TRISE_TRISE;
    I2C1->TRISE |=  ((APB1_CLK_MHZ + 1U) & I2C_TRISE_TRISE); // TRISE

    I2C1->CR1 |= I2C_CR1_PE; // enable i2c1 peripheral
    I2C1->CR1 |= I2C_CR1_ACK; // enable ACK
}

static void i2c1_irq_init(void) {
	NVIC_SetPriority(I2C1_EV_IRQn, I2C1_EV_PRIORITY);
	NVIC_ClearPendingIRQ(I2C1_EV_IRQn);
	NVIC_EnableIRQ(I2C1_EV_IRQn);

	NVIC_SetPriority(I2C1_ER_IRQn, I2C1_ER_PRIORITY);
	NVIC_ClearPendingIRQ(I2C1_ER_IRQn);
	NVIC_EnableIRQ(I2C1_ER_IRQn);

	I2C1->CR2 |= I2C_CR2_ITEVTEN; // enable event interrupt
	I2C1->CR2 |= I2C_CR2_ITERREN; // enable error interrupt
}
