/*
 * at24c32.h
 *
 *  Created on: Nov 6, 2023
 *      Author: Jim Merkle
 */

#ifndef _AT24C32_H_
#define _AT24C32_H_

// Defines:
#define I2C_ADDRESS_AT24C32	0x57	// This can be any address in the range 0x50 through 0x57, depending on A2:A0 pin strapping
#define AT24C32_BYTE_COUNT  4096    // The at24c32 is a 4K byte device (32Kbit)
#define AT24C32_PAGE_WRITE_SIZE 32  // Up to 32 bytes for a page write

int cl_read_at24c32(void);
int cl_write_at24c32(void);
int cl_fill_at24c32(void);
int cl_dump_at24c32(void);
int cl_write_at24c32_256(void);

void lame_dump(uint8_t * address, uint32_t count);

#endif /* _AT24C32_H_ */
