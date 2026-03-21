/*
 * bme280_processing.c
 *
 *  Created on: Mar 11, 2026
 *      Author: james
 *
 */

#include "bme280.h"
#include "stm32f401xe.h"
#include "config.h"

/* private API declarations */
static void bme280_compensate_temp(const bme280_raw_t* raw, const bme280_calib_t* calib);
static void bme280_compensate_press(const bme280_raw_t* raw, const bme280_calib_t* calib);
static void bme280_compensate_hum(const bme280_raw_t* raw, const bme280_calib_t* calib);

/* static variables */
static bme280_data_t data;
static int32_t t_fine;
/* public API definitions */
bme280_status_t bme280_compensate(void) {
	const bme280_raw_t* raw = bme280_get_raw();
	const bme280_calib_t* calib = bme280_get_calib();

	bme280_compensate_temp(raw, calib);
	bme280_compensate_press(raw, calib);
	bme280_compensate_hum(raw, calib);

	return BME280_STATUS_OK;
}

bme280_data_t* bme280_get_data(void) {
	return &data;
}

/* private API definitions */
static void bme280_compensate_temp(const bme280_raw_t *raw, const bme280_calib_t *calib)
{
    int32_t var1, var2;
    int32_t adc_T = (int32_t)raw->temp_raw;

    var1 = ((adc_T >> 3) - ((int32_t)calib->dig_T1 << 1));
    var1 = (var1 * (int32_t)calib->dig_T2) >> 11;

    var2 = (adc_T >> 4) - (int32_t)calib->dig_T1;
    var2 = (var2 * var2) >> 12;
    var2 = (var2 * (int32_t)calib->dig_T3) >> 14;

    t_fine = var1 + var2;
    data.temperature = (t_fine * 5 + 128) >> 8;
}

static void bme280_compensate_press(const bme280_raw_t *raw, const bme280_calib_t *calib)
{
    int64_t var1, var2, p;

    var1 = (int64_t)t_fine - 128000;
    var2 = var1 * var1 * (int64_t)calib->dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib->dig_P5) << 17);
    var2 = var2 + ((int64_t)calib->dig_P4 << 35);
    var1 = ((var1 * var1 * (int64_t)calib->dig_P3) >> 8) +
           ((var1 * (int64_t)calib->dig_P2) << 12);
    var1 = (((int64_t)1 << 47) + var1) * (int64_t)calib->dig_P1 >> 33;

    if(var1 == 0) {                  /* avoid divide by zero */
        data.pressure = 0U;
        return;
    }

    p    = 1048576 - (int64_t)raw->press_raw;
    p    = (((p << 31) - var2) * 3125) / var1;
    var1 = ((int64_t)calib->dig_P9 * (p >> 13) * (p >> 13)) >> 25;
    var2 = ((int64_t)calib->dig_P8 * p) >> 19;
    p    = ((p + var1 + var2) >> 8) + ((int64_t)calib->dig_P7 << 4);
    data.pressure = (uint32_t)(p / 256);
}

static void bme280_compensate_hum(const bme280_raw_t *raw, const bme280_calib_t *calib)
{
    int32_t v;

    v = t_fine - (int32_t)76800;
    v = (((((int32_t)raw->hum_raw << 14) -
           ((int32_t)calib->dig_H4 << 20) -
           ((int32_t)calib->dig_H5 * v)) + (int32_t)16384) >> 15) *
        (((((((v * (int32_t)calib->dig_H6) >> 10) *
             (((v * (int32_t)calib->dig_H3) >> 11) + (int32_t)32768)) >> 10) +
           (int32_t)2097152) * (int32_t)calib->dig_H2 + 8192) >> 14);

    v = v - (((((v >> 15) * (v >> 15)) >> 7) * (int32_t)calib->dig_H1) >> 4);
    v = (v < 0) ? 0 : v;
    v = (v > 419430400) ? 419430400 : v;

    data.humidity = (uint32_t)((v >> 12) * 100 / 1024);
}
