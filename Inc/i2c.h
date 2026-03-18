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
	I2C_STATUS_ERR_INVAILD_INPUT,
	I2C_STATUS_ERR
}i2c_status_t; // enum for i2c status

typedef enum {
	I2C_OP_READ_REGS,
	I2C_OP_WRITE_REG
}i2c_op_t; // enum for operation type, reading or writing

typedef enum {
	// common path
	I2C_STATE_IDLE = 0,
	I2C_STATE_START_SENT,
	I2C_STATE_ADDR_W_SENT,
	I2C_STATE_REG_SENT,

	// write path exclusive states
	I2C_STATE_TX_VAL,

	// read path exclusive states
	I2C_STATE_START_R,
	I2C_STATE_ADDR_R_SENT,
	I2C_STATE_RX_1,
	I2C_STATE_RX_2,
	I2C_STATE_RX_BULK,
	I2C_STATE_RX_LAST2,

	// done state
	I2C_STATE_DONE

}i2c_state_t; // states for state machine

typedef struct {
	uint8_t addr7; // 7 bit address
	uint8_t reg; // register address to read from or write to

	uint8_t* rx_buf; // destination buffer
	size_t rx_len; // #bytes to read
	size_t rx_idx; // #bytes already read

	uint8_t tx_val; // write val

	i2c_op_t op; // operation type (r/w)
	volatile i2c_state_t state; // advanced by ISR
	volatile i2c_status_t status;
	volatile uint8_t done;
}i2c_context_t; // struct to store current context of i2c

void i2c1_init(void);

i2c_status_t i2c1_write_reg(uint8_t addr7, uint8_t reg, uint8_t val);
i2c_status_t i2c1_read_regs(uint8_t addr7, uint8_t reg, uint8_t* buf, size_t n);

i2c_state_t i2c1_get_state(void);
uint8_t i2c1_is_done(void);

#endif /* I2C_H_ */
