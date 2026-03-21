/*
 * bme280.h
 *
 *  	Created on: Mar 11, 2026
 *      Author: james
 *
 *	  	bme280_init() :
 *	  	 	- reset chip to know state, verify chip ID, get calibration data
 *	  	      configure sample rate.
 *
 *	  	bme280_read_raw() :
 *	    	- check device not mid conversion, burst reads raw ADC values into
 *	      	  internal raw struct.
 *
 *	   	bme280_compensate() :
 *	   		- applies Bosch integer compensation formulas to internal raw ADC
 *	   	  	  counts using factory calibration coefficients, stores results
 *	   	      in internal data struct.
 *
 *	   	bme280_get_raw() :
 *	   		- retval : const bme280_raw_t*
 *	   		- returns pointer to internal raw ADC struct.
 *
 *	   	bme280_get_compensated() :
 *	   		- retval : const bme280_calib_t*
 *	   		- returns pointer to internal calibration struct.
 *
 *	    bme280_get_data() :
 *	    	- retval : const bme280_data_t*
 *	    	- returns pointer to internal compensated data struct.
 *
 *	    Note :  Output scaling conventions —
 *      		temperature : int32_t,  units of 0.01 °C  (2345 = 23.45 °C)
 *      		pressure    : uint32_t, units of 1 Pa      (101325 = 1013.25 hPa)
 *      		humidity    : uint32_t, units of 0.01 %RH  (5742 = 57.42 %RH)
 *
 *      		sample rate, oversampling, I2C address and register addresses are
 *      		controlled by macros in config.h.
 */

#ifndef BME280_H_
#define BME280_H_

#include <stdint.h>
#include <stddef.h>

/* status enum */
typedef enum {
    BME280_STATUS_OK,
    BME280_STATUS_BUSY,
    BME280_STATUS_ERR_WRONG_ID,
    BME280_STATUS_ERR_I2C,
    BME280_STATUS_ERR_INVALID_INPUT
} bme280_status_t;

/* raw ADC counts */
typedef struct {
    uint32_t press_raw;   // 20-bit pressure ADC value
    uint32_t temp_raw;    // 20-bit temperature ADC value
    uint16_t hum_raw;     // 16-bit humidity ADC value
} bme280_raw_t;

/* calibration data */
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

/* compensated data */
typedef struct {
	int32_t temperature; // 0.01°C
	uint32_t pressure; // 1 Pa
	uint32_t humidity; // 0.01 %RH
}bme280_data_t;

/* bme280_driver API */
bme280_status_t bme280_init(void);
bme280_status_t bme280_read_raw(void);

const bme280_calib_t* bme280_get_calib(void);
const bme280_raw_t* bme280_get_raw(void);

/* bme280_processing API */
bme280_status_t bme280_compensate(void);

const bme280_data_t* bme280_get_data(void);

#endif /* BME280_H_ */
