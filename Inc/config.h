/*
 * config.h
 *
 *  Created on: Mar 11, 2026
 *      Author: james
 */

#ifndef CONFIG_H_
#define CONFIG_H_
/* rcc */
#define SYSCLK_HZ          	16000000U
#define APB1_CLK_HZ 		16000000U
#define APB1_CLK_MHZ 	   	(APB1_CLK_HZ / 1000000U)
/* interrupt priority */
#define USART2_PRIORITY 	15
#define I2C1_EV_PRIORITY 	5
#define I2C1_ER_PRIORITY 	4
/* uart */
#define USART2_BAUDRATE 	115200U
#define USART2_TX_BUF_SIZE 	256U
/* i2c */
#define I2C1_SPEED_HZ      	100000U     /* Standard mode 100 kHz */
/* bme280 */
#define BME280_ADDR 		0x77U
#define BME280_ADDR_REG		0x60U
#define BME280_RESET_REG 	0xE0U
#define BME280_RESET_VAL	0xB6U

#endif /* CONFIG_H_ */
