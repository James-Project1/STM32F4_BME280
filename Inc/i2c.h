/*
 * i2c.h
 *
 *  Created on: Mar 11, 2026
 *      Author: james
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include <stddef.h>

typedef enum {
	I2C_STATUS_OK,
	I2C_STATUS_BUSY,
	I2C_STATUS_ERR
}i2c_status_t; // enum for i2c status

typedef enum {
	I2C_OP_READ_REGS,
	I2C_OP_WRITE_REG
}i2c_op_t; // enum for operation type, reading or writing

typedef enum {
	I2C_STATE_IDLE = 0,
	I2C_STATE_START_SENT,
	I2C_STATE_ADDR_W_SENT,
	I2C_STATE_REG_SENT

}i2c_state_t; // states for state machine

typedef struct {

}i2c_context_t; // struct to store current context of i2c

void i2c1_init(void);

i2c_status_t i2c1_write_reg(uint8_t addr7, uint8_t reg, uint8_t val);
i2c_status_t i2c1_read_regs(uint8_t addr7, uint8_t reg, uint8_t* buf, size_t n);

i2c_state_t i2c1_get_state(void);
uint8_t i2c1_is_done(void);

#endif /* I2C_H_ */
