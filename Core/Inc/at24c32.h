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


int cl_read_at24c32(void);
int cl_write_at24c32(void);
void lame_dump(uint8_t * address, uint32_t count);

#endif /* _AT24C32_H_ */
