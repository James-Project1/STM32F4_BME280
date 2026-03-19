/*
 * bme280.c
 *
 *  Created on: Mar 11, 2026
 *      Author: james
 *
 *      bme280_reset() :
 *      	- Puts chip into a guaranteed know state
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

/* public API definitions */
bme280_status_t bme280_init(void) {
	bme280_status_t status;

	if((status = bme280_reset()) != BME280_STATUS_OK)return status;
	if((status = bme280_check_id()) != BME280_STATUS_OK)return status;
	if((status = bme280_read_calib()) != BME280_STATUS_OK)return status;
	if((status = bme280_configure()) != BME280_STATUS_OK)return status;

	return BME280_STATUS_OK;
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

	delay_ms(10U);

	return BME280_STATUS_OK;
}

static bme280_status_t bme280_check_id(void) {
	uint8_t chip_id = 0U;
	i2c1_read_regs(BME280_ADDR, BME280_ADDR_REG, &chip_id, 1U);

		// non-blocking wait
	while(!i2c1_is_done());

	if(chip_id != 0x60U)return BME280_STATUS_ERR_WRONG_ID;

	return BME280_STATUS_OK;
}

static bme280_status_t bme280_read_calib(void) {
	return BME280_STATUS_OK;
}

static bme280_status_t bme280_configure(void) {
	return BME280_STATUS_OK;
}

