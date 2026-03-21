/*
 * uart.h
 *
 *  	Created on: Mar 11, 2026
 *      Author: james
 *
 *      usart_init() :
 *      	- init USART2 peripheral including baud rate, configure PA2
 *      	  as Tx line, init ring buffer. Tx interrupt only enabled
 *      	  when data is queued.
 *
 *      usart_write_bytes() :
 *      	- param[in] : buf
 *      	- param[in] : n
 *      	- Producer function, returns immediately does not wait for
 *      	  Tx to be complete. Validates inputs and ensures enough
 *      	  space in ring buffer for transfer.
 *
 *      Note : Baud rate, buffer size and NVIC priority are controlled by
 *      	   macros in config.h.
 *
 *      	   USART2_IRQHandler is consumer function, fires once per byte
 *      	   transmitted and drains the ring buffer until empty.
 *
 *      	   Rx is not enabled.
 */

#ifndef USART_H_
#define USART_H_

#include <stdint.h>
#include <stddef.h>

typedef enum{
	USART_STATUS_OK,
	USART_STATUS_ERR, // generic error case
	USART_STATUS_BUFF_FULL // insufficient space in ring buffer
} usart_status_t;

void usart2_init(void);
usart_status_t usart2_write_bytes(const uint8_t *buf, size_t n);

#endif /* USART_H_ */
