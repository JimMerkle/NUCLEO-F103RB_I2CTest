// Copyright Jim Merkle, 2/17/2020
// File: cl_i2c.h
//
// Defines, typedefs, structures for cl_i2c.c module
//
#ifndef _CL_I2C_H_
#define _CL_I2C_H_

#include "main.h"          // HAL functions and defines
#include "command_line.h"

#ifdef __cplusplus
extern "C" {
#endif

// HAL_I2C_MODULE_ENABLED will be defined when an I2C interface is enabled within the ioc file
#ifdef HAL_I2C_MODULE_ENABLED

#define I2C_ADDRESS_MIN	0x03
#define I2C_ADDRESS_MAX 0x77
#define I2C_SMALL_TIMEOUT  25  // More than enough time for 256 characters

// Externs:
extern I2C_HandleTypeDef hi2c1;


// Prototypes:
int cl_i2c_validate_address(uint16_t i2c_address); // I2C helper function that validates I2C address is within range
int cl_i2c_write_read(uint16_t i2c_address, uint8_t * pwrite, uint16_t wr_count, uint8_t * pread, uint16_t rd_count);
int cl_i2c_scan(void);
int cl_i2c_dump(void);
int cl_i2c_get(void);
int cl_i2c_set(void);

#endif // HAL_I2C_MODULE_ENABLED

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _CL_I2C_H_ */
