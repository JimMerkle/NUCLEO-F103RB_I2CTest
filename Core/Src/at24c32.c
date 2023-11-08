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
// Note: This function checks and manages address wrap that occurs on 32-byte boundaries
int at24c32_write(uint16_t address, uint8_t * data, uint16_t count)
{
	int rc;
	uint8_t buf[34]; // hold two bytes for storage address, and up to 32 bytes of data (page write)

	if(count > AT24C32_BYTE_COUNT) {
		printf("%s: count > %u\n",__func__,AT24C32_BYTE_COUNT);
		return 1;
	}
	while(count) {
		// Using the address provided, determine number of bytes we can write for the current page
		uint16_t next_page_boundary = (address + AT24C32_PAGE_WRITE_SIZE) & ~(AT24C32_PAGE_WRITE_SIZE-1);
		uint16_t bytes_this_page = next_page_boundary - address;
		uint16_t this_pass = count < bytes_this_page? count:bytes_this_page; // most bytes we can write for this pass

		// prepare for write - load up buf
		buf[0] = (uint8_t) (address >> 8); // address, high byte
		buf[1] = (uint8_t) address; // address, low byte
		memcpy(&buf[2],data,this_pass);
		rc = cl_i2c_write_read(I2C_ADDRESS_AT24C32, buf, this_pass+2, NULL, 0);
		if(rc) {
			printf("Error writing at24c32\n");
		}
		// update for next pass
		address+=this_pass;
		data+=this_pass;
		count-=this_pass;
		HAL_Delay(10); // require some delay for write to complete
	} // while-loop
	return rc;
}

// Read array of bytes from the at24c32 device.
// Note: For device reads, address wrap will occur at the end of physical device storage
int at24c32_read(uint16_t address, uint8_t * data, uint16_t count)
{
	uint8_t addr[2]; // hold two bytes for storage address

//	if(count > 32) {
//		printf("%s: count > 32\n",__func__);
//		return 1;
//	}
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

// command line method to display <argument 1> count or 32 bytes from the device
int cl_read_at24c32(void) {
	int rc;
	uint16_t count = 32;
    if(argc > 1) {
    	count = (uint16_t) strtol(argv[1], NULL, 0); // allow user to use decimal or hex
    }
	uint8_t buf[32];
	uint16_t address = 0;
	while(count) {
		uint16_t this_pass = count < 32?count:32; // number of bytes to read this pass
		rc = at24c32_read(address, buf, this_pass);
		hexdump(buf,this_pass);
		// update for next pass
		address+=this_pass;
		count-=this_pass;
	} // while-loop
	return rc;
}

const char qbf[]={"The quick brown fox jumped over the lazy dog."}; // 45 + null

// 256 bytes of randomly generated bytes
// See: https://www.random.org/bytes/
const uint8_t randbytes[256]= {
	0x86, 0xc8, 0xdb, 0x64, 0x97, 0x58, 0xc7, 0xcc, 0x5d, 0x15, 0x78, 0xf0, 0xc3, 0x0f, 0x57, 0xf8,
	0x2d, 0xbb, 0x07, 0xe7, 0x09, 0xf1, 0x46, 0xd5, 0x4a, 0x5e, 0xe4, 0xd4, 0x5d, 0xd2, 0x4c, 0x4c,
	0xba, 0x0f, 0x99, 0xf7, 0x95, 0xf8, 0x97, 0x84, 0x1a, 0x3b, 0x05, 0x40, 0x2e, 0xaa, 0xd2, 0xe1,
	0x43, 0x2c, 0x18, 0x94, 0x54, 0xaf, 0x49, 0x6d, 0x0f, 0x4d, 0x01, 0x99, 0xdf, 0xe2, 0xbc, 0x94,
	0xfd, 0x5d, 0xa8, 0x61, 0x0c, 0x75, 0x1d, 0x75, 0x2f, 0x41, 0x68, 0x72, 0x3f, 0x3f, 0x5a, 0x37,
	0xab, 0xfa, 0xfe, 0x24, 0x8f, 0x28, 0x8c, 0x70, 0xba, 0x60, 0xf4, 0xdb, 0x28, 0x7b, 0xa2, 0x35,
	0x54, 0x32, 0x82, 0x1f, 0x3d, 0x12, 0x32, 0x65, 0x57, 0x54, 0xe1, 0x2a, 0xf2, 0x5a, 0xd7, 0xd4,
	0x5d, 0x9b, 0x11, 0x07, 0xb0, 0xb4, 0xc3, 0x70, 0x18, 0x40, 0x17, 0x6b, 0x5d, 0x87, 0x1f, 0xd8,
	0x47, 0xf5, 0xc5, 0x16, 0xda, 0x6b, 0xc1, 0x53, 0xfe, 0x27, 0xac, 0xba, 0x18, 0xdb, 0x39, 0x4a,
	0x49, 0x86, 0x14, 0xbb, 0x67, 0x51, 0x25, 0x9e, 0xf9, 0xc4, 0xb8, 0x56, 0x7f, 0x9f, 0xaa, 0x5a,
	0xb9, 0xd6, 0xb2, 0x30, 0xfb, 0xf1, 0x00, 0x69, 0xb8, 0x92, 0xf5, 0x93, 0x01, 0x7a, 0xc6, 0x21,
	0xf0, 0x43, 0x76, 0xaa, 0xfd, 0x6c, 0xfb, 0xc9, 0x3d, 0xde, 0x1d, 0x3e, 0x1a, 0x7f, 0x01, 0x4f,
	0xb9, 0x38, 0xe8, 0xef, 0x3f, 0x80, 0x7c, 0xbd, 0xe1, 0xa5, 0xa4, 0xb6, 0x8b, 0x75, 0x3f, 0x55,
	0xd1, 0xd7, 0x0e, 0xa7, 0x9f, 0x64, 0xb5, 0x36, 0x2b, 0xc4, 0x34, 0xd4, 0x16, 0x01, 0xc4, 0x93,
	0x11, 0x80, 0xbe, 0xfe, 0x0f, 0x2c, 0xb8, 0x84, 0x06, 0xe6, 0xbd, 0xca, 0x15, 0xe6, 0x45, 0x10,
	0x8f, 0x4e, 0xd7, 0x86, 0x5e, 0xc0, 0x31, 0x09, 0x4f, 0x9c, 0x58, 0xdc, 0x57, 0x60, 0xa0, 0x60};

// command line method to write first 45 bytes in the device with "quick brown fox"
int cl_write_at24c32(void) {
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
#if 0
	// This section of code does a good job of filling the device, 32 bytes at a time
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
#else
	// Fill the device, 256 bytes at a time
	uint8_t buf[256];
	// Write the 256 byte buffer with incrementing data
	for(uint16_t i = 0;i<256;i++)
		buf[i] = i;
	// Write 256 bytes at a time until full (16 writes)
	for(uint16_t addr=0;addr<AT24C32_BYTE_COUNT;addr+=256) {
		rc = at24c32_write(addr, buf, sizeof(buf));
		if(rc) return rc;
		printf("."); // visual indicator for writing progress
	} // for-loop
#endif
	printf("\n");
	return rc;
}

// command line method to write 256 bytes to some address and then read it back and compare
int cl_write_at24c32_256(void) {
	int rc = at24c32_write(0x457, (uint8_t *)randbytes, sizeof(randbytes));
	if(rc) return rc;
	uint8_t readbuf[256];
	rc = at24c32_read(0x457, readbuf, sizeof(readbuf));
	if(rc) return rc;
	if(memcmp(randbytes,readbuf,sizeof(readbuf)))
		printf("Compare fail!\n");
	else {
		printf("Compare success!\n");
		hexdump(readbuf,sizeof(readbuf));
	}
	return rc;
}
