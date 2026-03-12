/*
 * uart.h
 *
 *  Created on: Mar 11, 2026
 *      Author: james
 */

#ifndef USART_H_
#define USART_H_

#include <stdint.h>
#include <stddef.h>

typedef enum{
	USART_STATUS_OK,
	USART_STATUS_TIMEOUT,
	USART_STATUS_ERR,
	USART_STATUS_BUFF_FULL
} uart_status_t;

void usart2_init();

uart_status_t usart2_write_bytes(const uint8_t *buf, size_t n);

#endif /* USART_H_ */
