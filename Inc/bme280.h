/*
 * bme280.h
 *
 *  Created on: Mar 11, 2026
 *      Author: james
 */

#ifndef BME280_H_
#define BME280_H_

// status enum
typedef enum {
    BME280_STATUS_OK,
    BME280_STATUS_BUSY,
    BME280_STATUS_ERR_WRONG_ID,
    BME280_STATUS_ERR_I2C,
    BME280_STATUS_ERR_INVALID_INPUT
} bme280_status_t;

// raw ADC counts


// calibration data


// compensated data


/* bme280_driver API */
/*
 * 	bme280_init()
 * 	 - Reset chip to know state
 *   - Reads chip ID register (0xD0), verifies 0x60
 *   - Reads all 24 bytes of trimming parameters from calib registers
 *     and stores them internally
 *   - Writes config: oversampling, mode, filter, standby time
 *   - Returns BME280_STATUS_ERR_WRONG_ID if chip is absent/wrong
 */
bme280_status_t bme280_init(void);

/* bme280_processing API */

#endif /* BME280_H_ */
