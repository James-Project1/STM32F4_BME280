/*
 * uart.h
 *
 *  	Created on: Mar 11, 2026
 *      Author: james
 *
 *      usart_init() :
 *      	- param[in] : NONE
 *      	- retval 	: NONE
 *
 *      usart_write_bytes() :
 *      	- param[in] : buf 				Pointer to the data to transmit. Must not be NULL.
 *      	- param[in] : n 				Number of bytes to transmit. Must be > 0.
 *      	- retval 	: usart_status_t	enumerated value
 *
 *      Note : Baud rate, buffer size and NVIC priority are controlled by
 *      	   macros in config.h.
 */

#ifndef USART_H_
#define USART_H_

#include <stdint.h>
#include <stddef.h>

typedef enum{
	USART_STATUS_OK, // no errors thrown, does not guarantee successful Tx
	USART_STATUS_ERR, // invalid argument or unrecoverable peripheral
	USART_STATUS_BUFF_FULL // insufficient space in ring buffer
} usart_status_t;

void usart2_init(void);

usart_status_t usart2_write_bytes(const uint8_t *buf, size_t n);

#endif /* USART_H_ */
