/*
 * uart.c
 *
 *  Created on: Mar 11, 2026
 *  Author: james
 *
 *  usart2_init() - Initialise uart2 peripheral for interrupt-driven transmission
 *  			 - Split into four stages, clocks, GPIO, USART2, Interrupt (NVIC)
 *
 *  usart2_write_bytes() - Public API called when tx of n bytes is required to host PC, producer function
 *  					- Interrupt-driven Tx
 *  					- returns immediately, does not stall CPU
 *
 *  USART2_IRQHandler() - Consumer function, hardware controls entry via vector table
 *  					- Executes once per byte transmitted
 */
/* includes */
#include "usart.h"
#include "stm32f401xe.h"
#include "config.h"

/* private API */
static void usart2_clock_init(void);
static void usart2_gpio_init(void);
static void usart2_configure(void);
static void usart2_irq_init(void);

/* USART IRQ Handler */
void USART2_IRQHandler(void);

/* Static variables */
/* ring buffer */
static volatile uint8_t tx_buf[USART2_TX_BUF_SIZE];
static volatile uint16_t head, tail;
/* Tx active flag */
static volatile uint8_t tx_active; /* 0 = idle, 1 = active */

/* public API definitions */
void usart2_init(){
	usart2_clock_init();
	usart2_gpio_init();
	usart2_configure();
	usart2_irq_init();
}

usart_status_t usart2_write_bytes(const uint8_t *buf, size_t n) {
	if(buf == NULL || n == 0)return USART_STATUS_ERR;

	uint32_t irq_state = NVIC_GetEnableIRQ(USART2_IRQn);
	NVIC_DisableIRQ(USART2_IRQn); // USART IRQ disabled

	/* compute free space in buffer is sufficient */
	uint16_t used;
	if(head >= tail)used = head - tail;
	else used = USART2_TX_BUF_SIZE - (tail - head);

	uint16_t free = (USART2_TX_BUF_SIZE - 1U) - used;

	if(n > (size_t)free) {
		NVIC_EnableIRQ(USART2_IRQn);
		return USART_STATUS_BUFF_FULL; // not enough space in buffer
	}
	// enqueue tx_buf
	for(size_t i = 0; i < n; i++){
		tx_buf[head] = buf[i];
		head++;
		if(head >= USART2_TX_BUF_SIZE)head = 0;
	}

	if(tx_active == 0U) {
		tx_active = 1U;
		USART2->CR1 |= USART_CR1_TXEIE; // set TXEIE
	}

	if(irq_state)NVIC_EnableIRQ(USART2_IRQn); // USART IRQ enabled

	return USART_STATUS_OK;
}

/* USART2 handler definition */
void USART2_IRQHandler(void) {
	// TXE = 1 & TXEIE  = 1
	if((USART2->CR1 & USART_CR1_TXEIE) && (USART2->SR & USART_SR_TXE)) {
		if(tail != head) {
			USART2->DR = (uint8_t)tx_buf[tail]; // write byte to DR
			tail++;
			if(tail >= USART2_TX_BUF_SIZE)tail = 0;
		}
		else {
			// tail == head, buffer empty no more data stop IRQ
			USART2->CR1 &= ~USART_CR1_TXEIE;
			tx_active = 0;
		}
	}
}

/* private API definitions */
static void usart2_clock_init(void) {
	// enable GPIOA peripheral clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	(void)RCC->AHB1ENR;
	// enable USART2 clock
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	(void)RCC->APB1ENR;
}

static void usart2_gpio_init(void) {
	/* PA2 Configuration Tx */
	// assign alternate function to PA2 and configure
	GPIOA->MODER &= ~(3U << 4U);
	GPIOA->MODER |= (2U << 4U); // set AF mode
	GPIOA->OTYPER &= ~(1U << 2U); // PA2 push-pull
	GPIOA->OSPEEDR &= ~(3U << 4U);
	GPIOA->OSPEEDR |= (2U << 4U); // PA2 Medium Speed
	GPIOA->PUPDR &= ~(3U << 4U); // no pull up/ pull down
	GPIOA->AFR[0] &= ~(0xF << 8);
	GPIOA->AFR[0] |= (7U << 8U); // set AF7

	/* PA3 Configuration Rx */
	// assign alternate function to PA3
	GPIOA->MODER &= ~(3U << 6U);
	GPIOA->MODER |= (2U << 6U); // set AF
	GPIOA->PUPDR &= ~(3U << 6U);
	GPIOA->PUPDR |= (1U << 6U); // set pull up
	GPIOA->AFR[0] &= ~(0xF << 12);
	GPIOA->AFR[0] |= (7U << 12U); // set AF7
}

static void usart2_configure(void) {
	// disable UART2 while configuring
	USART2->CR1 &= ~USART_CR1_UE;

	USART2->CR1 &= ~USART_CR1_OVER8; // over sample by 16
	USART2->CR1 &= ~USART_CR1_M; // 8 data bits
	USART2->CR1 &= ~USART_CR1_PCE; // no parity
	USART2->CR2 &= ~USART_CR2_STOP; // 1 stop bit

	/* Baud Rate calculation - Over sample x16 */
	uint32_t pclk = APB1_CLK_HZ;
	uint32_t baud = USART2_BAUDRATE;
	uint32_t usartdiv_times16 = (pclk + (baud / 2U)) / baud;
	uint32_t mantissa = usartdiv_times16 / 16U;
	uint32_t fraction = usartdiv_times16 % 16U;

	USART2->BRR = (mantissa << 4U) | (fraction & 0xFU); // write to baud rate register

	// enable USART2
	USART2->CR1 |= USART_CR1_TE; // TE
	// USART2->CR1 |= USART_CR1_RE; // RE , remove comment to enable Rx
	USART2->CR1 |= USART_CR1_UE;
	// Flush stale status
	(void)USART2->SR;
	(void)USART2->DR;
}

static void usart2_irq_init(void) {

	USART2->CR1 &= ~USART_CR1_TXEIE; // clear TXEIE (only set when queued data)

	head = 0;
	tail = 0;
	tx_active = 0;

	NVIC_SetPriority(USART2_IRQn, USART2_PRIORITY); // set priority low
	NVIC_ClearPendingIRQ(USART2_IRQn);
	NVIC_EnableIRQ(USART2_IRQn); // enable USART2 IRQ in NVIC
}


