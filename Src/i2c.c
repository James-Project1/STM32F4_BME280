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

/* private API declaration */
static void i2c1_clock_init(void);
static void i2c1_gpio_init(void);
static void i2c1_configure(void);
static void i2c1_irq_init(void);
/* I2C IRQ Handler declaration */
void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);
/* static declarations */
static volatile i2c_context_t handle;
/* public API definitions */
void i2c1_init(void) {
	i2c1_clock_init();
	i2c1_gpio_init();
	i2c1_configure();
	i2c1_irq_init();
}

i2c_status_t i2c1_read_regs(uint8_t addr7, uint8_t reg, uint8_t* buf, size_t n) {
	if(buf == NULL || n == 0U)return I2C_STATUS_ERR_INVALID_INPUT;
	if(addr7 > 0x7FU)return I2C_STATUS_ERR_INVALID_INPUT;
	if(handle.state != I2C_STATE_IDLE && handle.state != I2C_STATE_DONE)return I2C_STATUS_BUSY;

	handle.addr7 = addr7;
	handle.reg = reg;

	handle.rx_buf = buf;
	handle.rx_len = n;
	handle.rx_idx = 0;

	handle.op = I2C_OP_READ_REGS;

	handle.state = I2C_STATE_START_SENT;
	handle.status = I2C_STATUS_OK;
	handle.done = 0U;

	I2C1->CR1 |= I2C_CR1_ACK;
	I2C1->CR1 |= I2C_CR1_START;

	return I2C_STATUS_OK;
}

i2c_status_t i2c1_write_reg(uint8_t addr7, uint8_t reg, uint8_t val) {
	if(addr7 > 0x7FU)return I2C_STATUS_ERR_INVALID_INPUT;
	if(handle.state != I2C_STATE_IDLE && handle.state != I2C_STATE_DONE)return I2C_STATUS_BUSY;

	handle.addr7 = addr7;
	handle.reg = reg;

	handle.tx_val = val;

	handle.op = I2C_OP_WRITE_REG;

	handle.status = I2C_STATUS_OK;
	handle.state = I2C_STATE_START_SENT;
	handle.done = 0U;

	I2C1->CR1 |= I2C_CR1_START;

	return I2C_STATUS_OK;
}

i2c_state_t i2c1_get_state(void){
	return handle.state;
}

uint8_t i2c1_is_done(void) {
	return handle.done;
}

/* I2C IRQ Handler definitions */
void I2C1_EV_IRQHandler(void) {
	uint32_t sr1 = I2C1->SR1;
	switch(handle.state) {
	case I2C_STATE_START_SENT :
			if(sr1 & I2C_SR1_SB) {
				I2C1->DR = (uint8_t)((handle.addr7 << 1U) | 0); // 7 bit addr + write bit
				handle.state = I2C_STATE_ADDR_W_SENT;
			}
			break;

	case I2C_STATE_ADDR_W_SENT :
		if(sr1 & I2C_SR1_ADDR) {
			(void)I2C1->SR2; // dummy read to clear ADDR
			I2C1->DR = handle.reg;
			handle.state = I2C_STATE_REG_SENT;
		}
		break;

	case  I2C_STATE_REG_SENT :
		if(handle.op == I2C_OP_WRITE_REG) { // write path
			if(sr1 & I2C_SR1_TXE) {
				I2C1->DR = handle.tx_val;
				handle.state = I2C_STATE_TX_VAL;
			}
		}
		else {
			if((sr1 & I2C_SR1_BTF) && (sr1 & I2C_SR1_TXE)) { // read path
				I2C1->CR1 |= I2C_CR1_START; // generate repeated start
				handle.state = I2C_STATE_START_R;
			}
		}
		break;

	case I2C_STATE_TX_VAL :
		if((sr1 & I2C_SR1_BTF) && (sr1 & I2C_SR1_TXE)) {
			I2C1->CR1 |= I2C_CR1_STOP;
			I2C1->CR1 |= I2C_CR1_ACK;
			handle.done = 1U;
			handle.status = I2C_STATUS_OK;
			handle.state = I2C_STATE_DONE;
		}
		break;

	case I2C_STATE_START_R :
		if(sr1 & I2C_SR1_SB) {
			I2C1->DR = (uint8_t)((handle.addr7 << 1U) | 1U);
			handle.state = I2C_STATE_ADDR_R_SENT;
		}
		break;

	case I2C_STATE_ADDR_R_SENT :
		if(sr1 & I2C_SR1_ADDR) {
			if(handle.rx_len == 1U) {
				I2C1->CR1 &= ~I2C_CR1_ACK;
				(void)I2C1->SR1;
				(void)I2C1->SR2;
				I2C1->CR1 |= I2C_CR1_STOP;
				handle.state = I2C_STATE_RX_1;
			}

			else if(handle.rx_len == 2U) {
				I2C1->CR1 |= I2C_CR1_POS;
				I2C1->CR1 &= ~I2C_CR1_ACK;
				(void)I2C1->SR1;
				(void)I2C1->SR2;
				handle.state = I2C_STATE_RX_2;
			}

			else { // if rx_len >= 3
				I2C1->CR1 |= I2C_CR1_ACK;
				(void)I2C1->SR1;
				(void)I2C1->SR2;
				handle.state = I2C_STATE_RX_BULK;
			}
		}
		break;

	case I2C_STATE_RX_1 :
		if(sr1 & I2C_SR1_RXNE) {
			handle.rx_buf[handle.rx_idx++] = (uint8_t)I2C1->DR; // clears RXNE, executes STOP
			I2C1->CR1 |= I2C_CR1_ACK;
			handle.done = 1U;
			handle.status = I2C_STATUS_OK;
			handle.state = I2C_STATE_DONE;
		}
		break;

	case I2C_STATE_RX_2 :
		if(sr1 & I2C_SR1_BTF) {
			I2C1->CR1 |= I2C_CR1_STOP; // queue STOP
			handle.rx_buf[handle.rx_idx++] = (uint8_t)I2C1->DR; // first byte, shifts second into DR
			handle.rx_buf[handle.rx_idx++] = (uint8_t)I2C1->DR; // second byte
			I2C1->CR1 &= ~I2C_CR1_POS;
			I2C1->CR1 |= I2C_CR1_ACK;
			handle.done = 1U;
			handle.status = I2C_STATUS_OK;
			handle.state = I2C_STATE_DONE;
		}
		break;

	case I2C_STATE_RX_BULK : {
		size_t rem = handle.rx_len - handle.rx_idx;

		if(rem > 3U) {
			if(sr1 & I2C_SR1_RXNE) {
				handle.rx_buf[handle.rx_idx++] = (uint8_t)I2C1->DR;
			}
		}
		else if(rem == 3U) {
			if(sr1 & I2C_SR1_BTF) {
				I2C1->CR1 &= ~I2C_CR1_ACK;
				handle.rx_buf[handle.rx_idx++] = (uint8_t)I2C1->DR;
				handle.state = I2C_STATE_RX_2;
			}
		}
		break;
	}

	case I2C_STATE_DONE : // await next producer call
		break;

	default :
		break;

	}

}

void I2C1_ER_IRQHandler(void) {
    uint32_t sr1 = I2C1->SR1;

    /* clear all error flags — write 0 to each set bit */
    if(sr1 & I2C_SR1_BERR) I2C1->SR1 &= ~I2C_SR1_BERR;
    if(sr1 & I2C_SR1_ARLO) I2C1->SR1 &= ~I2C_SR1_ARLO;
    if(sr1 & I2C_SR1_AF)   I2C1->SR1 &= ~I2C_SR1_AF;
    if(sr1 & I2C_SR1_OVR)  I2C1->SR1 &= ~I2C_SR1_OVR;

    /* release the bus */
    I2C1->CR1 |= I2C_CR1_STOP;

    /* restore ACK and POS to known state */
    I2C1->CR1 &= ~I2C_CR1_POS;
    I2C1->CR1 |=  I2C_CR1_ACK;

    /* signal failure to caller */
    handle.status = I2C_STATUS_ERR;
    handle.done   = 1U;
    handle.state  = I2C_STATE_DONE;
}

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

	I2C1->CR2 |= I2C_CR2_ITEVTEN; // enable sequencing event interrupt
	I2C1->CR2 |= I2C_CR2_ITBUFEN; // enable buffer event interrupts
	I2C1->CR2 |= I2C_CR2_ITERREN; // enable error event interrupt
}
