// Copyright Jim Merkle, 11/01/2023
// File: cl_ds3231.h
//
// Defines, typedefs, structures for cl_ds3231.c module
//
#ifndef INC_CL_DS3231_H_
#define INC_CL_DS3231_H_

// Defines:
#define I2C_ADDRESS_DS3231	0x68

// Register definitions
#define DS_REG_SECONDS  0x00
#define DS_REG_DATE     0x04
#define DS_REG_STATUS	0x0F

// Status register bits
#define DS_STATUS_OSF   1<<7

// Prototypes:
uint8_t bcd_to_bin(uint8_t bcd);
uint8_t bin_to_bcd(uint8_t bin);
int cl_ds_time_valid(void);
int cl_ds_time(void);
int cl_ds_date(void);

#endif /* INC_CL_DS3231_H_ */
