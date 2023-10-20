// Copyright Jim Merkle, 2/17/2020
// Module: cl_i2c.c
//
// I2C routines for the command line interface
//

#include <stdio.h>
#include <stdint.h> // uint8_t
#include <stdlib.h> // strtol()
#include "main.h"   // HAL functions and defines - HAL_I2C_MODULE_ENABLED in stm32f1xx_hal_conf.h
#include "cl_i2c.h"

/* To implement the expected functionality, the following lines would be added to
   command_line.c, in the cmd_table[]:

#ifdef HAL_I2C_MODULE_ENABLED
	{"i2cscan",   "scan i2c bus for connected devices",           1, cl_i2c_scan},
	{"i2cdump",   "i2cdump <i2c address>",                        2, cl_i2c_dump},
	{"i2cget",    "i2cget <i2c address> <register>",              3, cl_i2c_get},
	{"i2cset",    "i2cset <i2c address> <register> <value>",      3, cl_i2c_set},
#endif // HAL_I2C_MODULE_ENABLED

*/

// HAL_I2C_MODULE_ENABLED will be set when an I2C interface is enabled within the ioc file
#ifdef HAL_I2C_MODULE_ENABLED
// Perform an I2C bus scan similar to Linux's i2cdetect, or Arduino's i2c_scanner sketch
int cl_i2c_scan(void)
{
    printf("I2C Scan - scanning I2C addresses 0x%02X - 0x%02X\n",I2C_ADDRESS_MIN,I2C_ADDRESS_MAX);
    // Display Hex Header
    printf("    "); for(int i=0;i<=0x0F;i++) printf(" %0X ",i);
    // Walk through address range 0x00 - 0x77, but only test 0x03 - 0x77
    for(int addr=0;addr<=I2C_ADDRESS_MAX;addr++) {
    	// If address defines the beginning of a row, start a new row and display row text
    	if(!(addr%16)) printf("\n%02X: ",addr);
		// Check I2C addresses in the range 0x03-0x7F
		if(addr < I2C_ADDRESS_MIN || addr > I2C_ADDRESS_MAX) {
			printf("   "); // out of range
			continue;
		}
		// Perform I2C device detection - returns HAL_OK if device found
		if(HAL_OK == HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(addr<<1), 2, 2))
			printf("%02X ",addr);
		else
			printf("-- ");
    } // for-loop
    printf("\n");
    return 0;
} // cl_i2c_scanner


// Read and display register content from I2C device
// Expect: "i2cdump <i2caddress>"
// By default, assume byte-wise register addressing, and display the first 16 registers
// The HAL_I2C_ APIs require an 8-bit addresses vs 7-bit address (shift left is required)
int cl_i2c_dump(void)
{
	HAL_StatusTypeDef hal_status;

	uint16_t i2c_address = strtol(argv[1],NULL,0); // allow user to use decimal or hex for address
	// Validate I2C address range
	if(i2c_address < I2C_ADDRESS_MIN || i2c_address > I2C_ADDRESS_MAX) {
		printf("Address out of range. Expect 0x%02X to 0x%02X\n",I2C_ADDRESS_MIN,I2C_ADDRESS_MAX); // out of range
		return -1;
	}
    // Display Hex Header
	for(int i=0;i<=0x0F;i++) printf(" %02X",i);
	printf("\n\n");
	// Write register address and then read the value
	uint8_t i2c_reg;
	uint8_t i2c_data;
	for(i2c_reg=0;i2c_reg<=0x0F;i2c_reg++) {
		hal_status = HAL_I2C_Master_Transmit(&hi2c1, i2c_address<<1, &i2c_reg, 1, I2C_SMALL_TIMEOUT);
		if(HAL_OK != hal_status) {printf("Error %d writing to I2C address 0x%02X\n",hal_status,i2c_address); return -2;}
		hal_status = HAL_I2C_Master_Receive(&hi2c1, i2c_address<<1, &i2c_data, 1, I2C_SMALL_TIMEOUT);
		if(HAL_OK != hal_status) {printf("Error %d reading from I2C address 0x%02X\n",hal_status,i2c_address); return -2;}
		printf(" %02X",i2c_data);
	}
	printf("\n");

	return 0;
}

// Read and display contents of a single byte-wise register
// Expect: "i2cget <i2caddress> <i2cregister>"
// The HAL_I2C_ APIs require an 8-bit addresses vs 7-bit address (shift left is required)
int cl_i2c_get(void)
{
	HAL_StatusTypeDef hal_status;

	uint16_t i2c_address = strtol(argv[1],NULL,0); // allow user to use decimal or hex for address
	uint8_t i2c_register = strtol(argv[2],NULL,0); // allow user to use decimal or hex for register
	// Validate I2C address range
	if(i2c_address < I2C_ADDRESS_MIN || i2c_address > I2C_ADDRESS_MAX) {
		printf("Address out of range. Expect 0x%02X to 0x%02X\n",I2C_ADDRESS_MIN,I2C_ADDRESS_MAX); // out of range
		return -1;
	}

	// Write register address and then read the value
	uint8_t i2c_data;
	hal_status = HAL_I2C_Master_Transmit(&hi2c1, i2c_address<<1, &i2c_register, 1, I2C_SMALL_TIMEOUT);
	if(HAL_OK != hal_status) {printf("Error %d writing to I2C address 0x%02X\n",hal_status,i2c_address); return -2;}
	hal_status = HAL_I2C_Master_Receive(&hi2c1, i2c_address<<1,  &i2c_data, 1, I2C_SMALL_TIMEOUT);
	if(HAL_OK != hal_status) {printf("Error %d reading from I2C address 0x%02X\n",hal_status,i2c_address); return -2;}
	printf("Addr 0x%02X, Reg 0x%02X, Value %02X\n",i2c_address,i2c_register,i2c_data);

	return 0;
}

// Write byte to a single register
// Expect: "i2cset <i2caddress> <i2cregister> <registervalue>"
// The HAL_I2C_ APIs require an 8-bit addresses vs 7-bit address (shift left is required)
int cl_i2c_set(void)
{
	HAL_StatusTypeDef hal_status;
	uint8_t buffer[2];

	uint16_t i2c_address = strtol(argv[1],NULL,0); // allow user to use decimal or hex for address
	// Read and load buffer with [0]:register, [1]:value
	buffer[0] = strtol(argv[2],NULL,0); // allow user to use decimal or hex for register
	buffer[1] = strtol(argv[3],NULL,0); // allow user to use decimal or hex for register
	// Validate I2C address range
	if(i2c_address < I2C_ADDRESS_MIN || i2c_address > I2C_ADDRESS_MAX) {
		printf("Address out of range. Expect 0x%02X to 0x%02X\n",I2C_ADDRESS_MIN,I2C_ADDRESS_MAX); // out of range
		return -1;
	}

	// Write register address and value
	hal_status = HAL_I2C_Master_Transmit(&hi2c1, i2c_address<<1, buffer, sizeof(buffer), I2C_SMALL_TIMEOUT);
	if(HAL_OK != hal_status) {printf("Error %d writing to I2C address 0x%02X\n",hal_status,i2c_address); return -2;}
	return 0;
}
#endif // HAL_I2C_MODULE_ENABLED
