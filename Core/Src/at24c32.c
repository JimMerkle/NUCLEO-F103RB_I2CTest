/*
 * at24c32.c
 *
 *  Created on: Nov 6, 2023
 *      Author: Jim Merkle
 */
#include "command_line.h"
#include "at24c32.h"
#include "cl_i2c.h"
#include <string.h> // memcpy()

// Write array of bytes to the at24c32 device, using "Page Write" method (up to 32 bytes of data written with one start and one stop).
// Note: This function doesn't check for address wrap that occurs on 32-byte boundaries
int at24c32_write(uint16_t address, uint8_t * data, uint8_t count)
{
	uint8_t buf[34]; // hold two bytes for storage address, and up to 32 bytes of data

	if(count > 32) {
		printf("%s: count > 32\n",__func__);
		return 1;
	}
	// prepare for write - load up buf
	buf[0] = (uint8_t) (address >> 8); // address, high byte
	buf[1] = (uint8_t) address; // address, low byte
	memcpy(&buf[2],data,count);
	int rc = cl_i2c_write_read(I2C_ADDRESS_AT24C32, buf, count+2, NULL, 0);
	if(rc) {
		printf("Error writing at24c32\n");
	}
	return rc;
}

// Read array of bytes from the at24c32 device.
// Note: For device reads, address wrap will occur at the end of physical device storage
int at24c32_read(uint16_t address, uint8_t * data, uint8_t count)
{
	uint8_t addr[2]; // hold two bytes for storage address

	if(count > 32) {
		printf("%s: count > 32\n",__func__);
		return 1;
	}
	// Write address to begin reading
	addr[0] = (uint8_t) (address >> 8); // address, high byte
	addr[1] = (uint8_t) address; // address, low byte
	int rc = cl_i2c_write_read(I2C_ADDRESS_AT24C32, addr, 2, data, count);
	if(rc) {
		printf("Error reading at24c32\n");
	}
	return rc;
}

#if 0
// Although lame_dump works, it's just a little too lame compared to hexdump()
// Display one or more rows, 16 bytes each
void lame_dump(uint8_t * address, uint32_t count)
{
	while (count) {
		uint32_t thisrow = count<=16? count:16; // display 1-16 bytes
		// print a row
		for(uint32_t i=0;i<thisrow;i++)
			printf("%02X ",*address++);
		// update count
		count-=thisrow;
		printf("\n");
	};
	printf("\n");
}
#endif

void hexdump(const void* address, unsigned size); // hexdump.c

// command line method to display first 32 bytes in the device
int cl_read_at24c32(void) {
	uint8_t buf[32];
	int rc = at24c32_read(0, buf, sizeof(buf));
	hexdump(buf,sizeof(buf));
	return rc;
}

//const char qbf[]={"The quick brown fox jumped."}; // 27 + null
const char qbf[]={"12345678901234567890123456789012"}; // 32 + null

// command line method to write first 32 bytes in the device
int cl_write_at24c32(void) {
	//int rc = at24c32_write(0, (uint8_t *)qbf, sizeof(qbf));
	int rc = at24c32_write(0, (uint8_t *)qbf, strlen(qbf)); // don't write the terminating null
	return rc;
}

// command line method to dump the contents of the at24c32 device
int cl_dump_at24c32(void) {
	int rc;
	uint8_t buf[32];
	for(uint16_t addr=0;addr<AT24C32_BYTE_COUNT;addr+=32) {
		rc = at24c32_read(addr, buf, sizeof(buf));
		hexdump(buf,sizeof(buf)); // this won't be the prettiest, since the address will be the same for each call
		if(rc) return rc;
	} // for-loop
	return rc;
}

// command line method to fill the device with values 0x00 through 0xFF
int cl_fill_at24c32(void) {
	int rc;
	uint8_t buf[AT24C32_PAGE_WRITE_SIZE];
	for(uint16_t addr=0;addr<AT24C32_BYTE_COUNT;addr+=32) {
		// Fill buf for each page write
		uint8_t data = (uint8_t)addr;
		for(uint16_t i = 0;i<AT24C32_PAGE_WRITE_SIZE;i++) buf[i] = data++;

		rc = at24c32_write(addr, buf, AT24C32_PAGE_WRITE_SIZE); // page write
		if(rc) return rc;
		printf(".");
		HAL_Delay(10); // some delay is required to complete the page write
	} // for-loop
	printf("\n");
	return rc;
}
