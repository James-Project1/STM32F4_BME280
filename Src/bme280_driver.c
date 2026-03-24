/*
 * bme280_driver.c
 *
 *  Created on: Mar 11, 2026
 *      Author: james
 *
 */
#include "bme280.h"
#include "stm32f401xe.h"
#include "config.h"
#include "i2c.h"
#include "systick.h"

/* private API declarations */
static bme280_status_t i2c_wait(void);
static bme280_status_t bme280_reset(void);
static bme280_status_t bme280_check_id(void);
static bme280_status_t bme280_read_calib(void);
static bme280_status_t bme280_configure(void);
/* static declarations */
static bme280_calib_t calib;
static bme280_raw_t raw;
/* public API definitions */
bme280_status_t bme280_init(void) {
	bme280_status_t status;

	if((status = bme280_reset()) != BME280_STATUS_OK)return status;
	if((status = bme280_check_id()) != BME280_STATUS_OK)return status;
	if((status = bme280_read_calib()) != BME280_STATUS_OK)return status;
	if((status = bme280_configure()) != BME280_STATUS_OK)return status;

	return BME280_STATUS_OK;
}

bme280_status_t bme280_read_raw(void) {
    uint8_t buf[BME280_DATA_LEN];

    if(i2c1_read_regs(BME280_ADDR, BME280_DATA_REG, buf, BME280_DATA_LEN) != I2C_STATUS_OK)return BME280_STATUS_ERR_I2C;
    if(i2c_wait() != BME280_STATUS_OK) return BME280_STATUS_ERR_I2C;

    raw.press_raw = ((uint32_t)buf[0] << 12) | ((uint32_t)buf[1] <<  4) | ((uint32_t)buf[2] >>  4);
    raw.temp_raw  = ((uint32_t)buf[3] << 12) | ((uint32_t)buf[4] <<  4) | ((uint32_t)buf[5] >>  4);
    raw.hum_raw   = ((uint16_t)buf[6] <<  8) | (uint16_t)buf[7];

    return BME280_STATUS_OK;
}

const bme280_calib_t* bme280_get_calib(void) {
	return &calib;
}

const bme280_raw_t* bme280_get_raw(void) {
	return &raw;
}

/* private API definitions */
static bme280_status_t i2c_wait(void) {
    while(!i2c1_is_done());
    if(i2c1_get_state() != I2C_STATE_DONE) return BME280_STATUS_ERR_I2C;

    // wait for bus to fully release after STOP
    uint32_t timeout = 10000U;
    while((I2C1->SR2 & I2C_SR2_BUSY) && timeout){
        timeout--;
    }

    if(I2C1->SR2 & I2C_SR2_BUSY) return BME280_STATUS_ERR_I2C;

    return BME280_STATUS_OK;
}

static bme280_status_t bme280_reset(void) {
	if(i2c1_write_reg(BME280_ADDR, BME280_RESET_REG, BME280_RESET_VAL) != I2C_STATUS_OK)return BME280_STATUS_ERR_I2C;

	if(i2c_wait() != BME280_STATUS_OK) return BME280_STATUS_ERR_I2C;

	delay_ms(5U);

	return BME280_STATUS_OK;
}

static bme280_status_t bme280_check_id(void) {
	uint8_t chip_id = 0U;
	if(i2c1_read_regs(BME280_ADDR, BME280_CHIP_ID_REG, &chip_id, 1U) != I2C_STATUS_OK)return BME280_STATUS_ERR_I2C;

	if(i2c_wait() != BME280_STATUS_OK) return BME280_STATUS_ERR_I2C;

	if(chip_id != BME280_CHIP_ID)return BME280_STATUS_ERR_WRONG_ID;

	return BME280_STATUS_OK;
}

static bme280_status_t bme280_read_calib(void) {
	uint8_t bank1[BME280_CALIB_00_LEN];   /* 26 bytes: 0x88–0xA1 */
	uint8_t bank2[BME280_CALIB_26_LEN];   /*  7 bytes: 0xE1–0xE7 */

	/* bank 1 */
	if(i2c1_read_regs(BME280_ADDR, BME280_CALIB_00_REG, bank1, BME280_CALIB_00_LEN) != I2C_STATUS_OK) return BME280_STATUS_ERR_I2C;
	if(i2c_wait() != BME280_STATUS_OK) return BME280_STATUS_ERR_I2C;

	calib.dig_T1 = (uint16_t)(((uint16_t)bank1[1] << 8) | bank1[0]);
	calib.dig_T2 = (int16_t)(((uint16_t)bank1[3] << 8) | bank1[2]);
	calib.dig_T3 = (int16_t)(((uint16_t)bank1[5] << 8) | bank1[4]);
    calib.dig_P1 = (uint16_t)(((uint16_t)bank1[7]  << 8) | bank1[6]);
    calib.dig_P2 = (int16_t)(((uint16_t)bank1[9]  << 8) | bank1[8]);
    calib.dig_P3 = (int16_t)(((uint16_t)bank1[11] << 8) | bank1[10]);
    calib.dig_P4 = (int16_t)(((uint16_t)bank1[13] << 8) | bank1[12]);
    calib.dig_P5 = (int16_t)(((uint16_t)bank1[15] << 8) | bank1[14]);
    calib.dig_P6 = (int16_t)(((uint16_t)bank1[17] << 8) | bank1[16]);
    calib.dig_P7 = (int16_t)(((uint16_t)bank1[19] << 8) | bank1[18]);
    calib.dig_P8 = (int16_t)(((uint16_t)bank1[21] << 8) | bank1[20]);
    calib.dig_P9 = (int16_t)(((uint16_t)bank1[23] << 8) | bank1[22]);
    calib.dig_H1 = bank1[25];

    /* bank 2 */
    if(i2c1_read_regs(BME280_ADDR, BME280_CALIB_26_REG, bank2, BME280_CALIB_26_LEN) != I2C_STATUS_OK) return BME280_STATUS_ERR_I2C;
    if(i2c_wait() != BME280_STATUS_OK) return BME280_STATUS_ERR_I2C;

    calib.dig_H2 = (int16_t)(((uint16_t)bank2[1] << 8) | bank2[0]);
    calib.dig_H3 = bank2[2];
    calib.dig_H4 = ((int16_t)bank2[3] << 4) | (int16_t)(bank2[4] & 0x0FU);
    calib.dig_H5 = ((int16_t)bank2[5] << 4) | (int16_t)(bank2[4] >> 4);
    calib.dig_H6 = (int8_t)bank2[6];

    return BME280_STATUS_OK;
}

static bme280_status_t bme280_configure(void) {
	if(i2c1_write_reg(BME280_ADDR, BME280_CTRL_HUM_REG, BME280_CTRL_HUM_VAL) != I2C_STATUS_OK)return BME280_STATUS_ERR_I2C;
    if(i2c_wait() != BME280_STATUS_OK) return BME280_STATUS_ERR_I2C;

    if(i2c1_write_reg(BME280_ADDR, BME280_CTRL_MEAS_REG, BME280_CTRL_MEAS_VAL) != I2C_STATUS_OK)return BME280_STATUS_ERR_I2C;
    if(i2c_wait() != BME280_STATUS_OK) return BME280_STATUS_ERR_I2C;

    if(i2c1_write_reg(BME280_ADDR, BME280_CONFIG_REG, BME280_CONFIG_VAL) != I2C_STATUS_OK)return BME280_STATUS_ERR_I2C;
    if(i2c_wait() != BME280_STATUS_OK) return BME280_STATUS_ERR_I2C;

    return BME280_STATUS_OK;
}

