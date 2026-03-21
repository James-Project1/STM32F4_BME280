/*
 * bme280.h
 *
 *  Created on: Mar 11, 2026
 *      Author: james
 *
 * 	bme280_init()
 * 	 - Reset chip to know state
 *   - Reads chip ID register (0xD0), verifies 0x60
 *   - Reads all 24 bytes of trimming parameters from calib registers
 *     and stores them internally
 *   - Writes config: oversampling, mode, filter, standby time
 *   - Returns BME280_STATUS_ERR_WRONG_ID if chip is absent/wrong
 *
 * 	bme280_trigger_measurement()
 *   - Only relevant in forced mode
 *   - Writes forced mode bit to ctrl_meas, kicks off one shot
 *   - Returns BUSY if a prior transfer is still in flight
 *
 * 	bme280_read_raw()
 *   - Burst-reads the six ADC result registers (0xF7–0xFC)
 *     for pressure and temperature, plus 0xFD–0xFE for humidity
 *   - Stores results in the internal context for processing.c to consume
 *   - Non-blocking: starts the I2C transfer and returns immediately
 */

#ifndef BME280_H_
#define BME280_H_

#include <stdint.h>
#include <stddef.h>

// status enum
typedef enum {
    BME280_STATUS_OK,
    BME280_STATUS_BUSY,
    BME280_STATUS_ERR_WRONG_ID,
    BME280_STATUS_ERR_I2C,
    BME280_STATUS_ERR_INVALID_INPUT
} bme280_status_t;

// raw ADC counts
typedef struct {
    uint32_t press_raw;   /* 20-bit pressure ADC value */
    uint32_t temp_raw;    /* 20-bit temperature ADC value */
    uint16_t hum_raw;     /* 16-bit humidity ADC value */
} bme280_raw_t;

// calibration data
typedef struct {
    /* temperature */
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    /* pressure */
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

    /* humidity */
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
} bme280_calib_t;

// compensated data
typedef struct {
	int32_t temperature;
	uint32_t pressure;
	uint32_t humidity;
}bme280_data_t;

/* bme280_driver API */
bme280_status_t bme280_init(void);
bme280_status_t bme280_trigger_measurement(void);
bme280_status_t bme280_read_raw(void);

uint8_t bme280_is_done(void);
const bme280_calib_t* bme280_get_calib(void);
const bme280_raw_t* bme280_get_raw(void);

/* bme280_processing API */
bme280_status_t bme280_compensate(void);

bme280_data_t* bme280_get_data(void);

#endif /* BME280_H_ */
