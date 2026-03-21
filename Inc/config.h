/*
 * config.h
 *
 *  Created on: Mar 11, 2026
 *      Author: james
 */

#ifndef CONFIG_H_
#define CONFIG_H_
/* rcc */
#define SYSCLK_HZ          		16000000U
#define APB1_CLK_HZ 			16000000U
#define APB1_CLK_MHZ 	   		(APB1_CLK_HZ / 1000000U)
/* interrupt priority */
#define USART2_PRIORITY 		15
#define I2C1_EV_PRIORITY 		5
#define I2C1_ER_PRIORITY 		4
/* uart */
#define USART2_BAUDRATE 		115200U
#define USART2_TX_BUF_SIZE 		256U
/* i2c */
#define I2C1_SPEED_HZ      		100000U     /* Standard mode 100 kHz */
/* bme280 */
/* device */
#define BME280_ADDR 			0x77U
#define BME280_CHIP_ID 			0x60U
#define BME280_CHIP_ID_REG 		0xD0U
/* reset */
#define BME280_RESET_REG        0xE0U
#define BME280_RESET_VAL        0xB6U
/* calibration banks */
#define BME280_CALIB_00_REG     0x88U   // bank 1, 24 bytes
#define BME280_CALIB_00_LEN     26U
#define BME280_CALIB_26_REG     0xE1U   // bank 2,  7 bytes
#define BME280_CALIB_26_LEN     7U
/* configuration */
#define BME280_CTRL_HUM_REG     0xF2U
#define BME280_CTRL_HUM_VAL 	0x01U
#define BME280_CTRL_MEAS_REG    0xF4U
#define BME280_CTRL_MEAS_VAL	0x24U
#define BME280_CONFIG_REG       0xF5U
#define BME280_CONFIG_VAL 		0x00U
#define BME280_MODE_FORCED 		0x25U
/* status */
#define BME280_STATUS_REG       0xF3U
#define BME280_STATUS_MEASURING 0x08U // (1U << 3)
/* raw data */
#define BME280_DATA_REG         0xF7U
#define BME280_DATA_LEN         8U

#endif /* CONFIG_H_ */
