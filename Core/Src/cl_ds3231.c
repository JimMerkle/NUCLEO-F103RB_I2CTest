// Copyright Jim Merkle, 11/01/2023
// File: cl_ds3231.c
//
// Implement various command line routines that interact with the DS3231 RTC
//
#include <stdio.h>
#include "command_line.h"
#include "cl_i2c.h"
#include "cl_ds3231.h"
#include "rtc_lib.h"

// Forward declarations:
int read_rtc_into_date_time(DATE_TIME * dt);


/* To implement the expected functionality, the following lines would be added to
   command_line.c, in the cmd_table[]:

#ifdef HAL_I2C_MODULE_ENABLED
    {"time",      "time <hrs min sec>",                           1, cl_ds_time},
    {"date",      "date <mm dd yy>",                              1, cl_ds_date},
    {"ts",        "Unix time (in seconds)",                       1, cl_ds_time_stamp},
#endif // HAL_I2C_MODULE_ENABLED
*/

// The clock and calendar registers use BCD (Binary Coded Decimal Format)
// Convert a BCD value to Binary
uint8_t bcd_to_bin(uint8_t bcd)
{
	// get high nibble, multiply it by 10, then add low nibble
	uint8_t bin = ((bcd >> 4) * 10) + (bcd & 0x0F);
	return bin;
}

// Convert a Binary value to BCD
uint8_t bin_to_bcd(uint8_t bin)
{
	// get high nibble, multiply it by 10, then add low nibble
	uint8_t bcd = ((bin/10)<<4) + (bin%10);
	return bcd;
}

// DS3231 helper function - is RTC time valid
// 0: time valid
int ds_time_valid(void)
{
	// Start with reading the Status Register (0Fh,. Bit 7: Oscillator Stop Flag (OSF).
	// If set, the oscillator was stopped in the past.  Time / date registers may be invalid.
	uint8_t reg=DS_REG_STATUS;
	uint8_t reg_value;
	int rc = cl_i2c_write_read(I2C_ADDRESS_DS3231, &reg, 1, &reg_value, 1); // read status register, 0x0F
	if(rc) {
		printf("Error reading DS3231 status register\n");
		return rc;
	}
	if(reg_value & DS_STATUS_OSF) {
		printf("OSF set - time invalid\n");
		return 1;
	}
	return 0;
}

// Check if DS3231 is present
// Returns 0 (HAL_OK) if present, else returns non-zero and displays error messages
int ds3231_present(I2C_HandleTypeDef *hi2c)
{
	HAL_StatusTypeDef rc = HAL_I2C_IsDeviceReady(hi2c, (uint16_t)(I2C_ADDRESS_DS3231<<1), 2, 2);
	if(HAL_OK != rc) printf("DS3231 not found!\n");
	return rc;
}

//
int cl_ds_time(void)
{
	// If DS3231 doesn't appear to be attached, return with error now
	int rc = ds3231_present(&hi2c1);
	if(HAL_OK != rc) return rc;

	uint8_t sec_min_hr[4];

	switch(argc) {
	case 4:
		// Three arguments - Write time to time registers, clear status register - OSF bit
		// Load buffer for I2C write - BCD format
		sec_min_hr[3] = bin_to_bcd((uint8_t)strtol(argv[1],NULL,0)); // hours - allow user to use decimal or hex for address
		sec_min_hr[2] = bin_to_bcd((uint8_t)strtol(argv[2],NULL,0)); // minutes
		sec_min_hr[1] = bin_to_bcd((uint8_t)strtol(argv[3],NULL,0)); // seconds
		sec_min_hr[0] = DS_REG_SECONDS; // begin writing to seconds register

		rc = cl_i2c_write_read(I2C_ADDRESS_DS3231, sec_min_hr, 4, NULL, 0); // write time registers
		if(rc) {
			printf("Error writing DS3231 time registers\n");
			return rc;
		}

		// Clear OSF status register bit
		uint8_t index_status[2] = {DS_REG_STATUS,0};
		rc = cl_i2c_write_read(I2C_ADDRESS_DS3231, index_status, 2, NULL, 0); // write status register
		if(rc) {
			printf("Error writing DS3231 status registers\n");
			return rc;
		}
		// Fall through - read registers and display time

	case 1:
		// No arguments - Display time - hours:minutes:seconds

		// Check RTC, status register - OSF flag.  Flag should be clear
		//	rc = ds_time_valid();
		//	if(rc) return rc;
#if 0
		// Read seconds, minutes, hours into buffer
		uint8_t reg=DS_REG_SECONDS;
		rc = cl_i2c_write_read(I2C_ADDRESS_DS3231, &reg, 1, sec_min_hr, 3); // read [0]seconds, [1]minutes, [2]hours
		if(rc) {
			printf("Error reading DS3231 seconds, minutes, hours registers\n");
			return rc;
		}
		printf("%d:%02d:%02d\n",bcd_to_bin(sec_min_hr[2]),bcd_to_bin(sec_min_hr[1]),bcd_to_bin(sec_min_hr[0]));
#else
		// Read the DS3231, return date and time via the DATE_TIME structure pointer
		DATE_TIME dt;
		read_rtc_into_date_time(&dt);
		// Convert the DATE_TIME structure value into a Linux number of seconds
		uint32_t utc_time = unixtime(&dt);
		// Subtract 5 hours of seconds from the time
		utc_time -= (5 * 60 * 60);
		// Convert back to DATE_TIME structure
		unix_to_date_time(&dt, utc_time);
		// Display local time
		printf("%d:%02d:%02d\n",dt.hours,dt.minutes,dt.seconds);
#endif
		break;

	default:
		printf("Invalid number of arguments\n");
		return 1;
	} // switch(argc)

	return 0;
} // cl_ds_time


int cl_ds_date(void)
{
	// If DS3231 doesn't appear to be attached, return with error now
	int rc = ds3231_present(&hi2c1);
	if(HAL_OK != rc) return rc;

	uint8_t reg=DS_REG_DATE;
	uint8_t date_month_year[4];

	switch(argc) {
	case 4:
		// Three arguments - Write date to calendar registers
		// Load buffer for I2C write - BCD format
		date_month_year[2] = bin_to_bcd((uint8_t)strtol(argv[1],NULL,0)); // month - allow user to use decimal or hex for address
		date_month_year[1] = bin_to_bcd((uint8_t)strtol(argv[2],NULL,0)); // date
		date_month_year[3] = bin_to_bcd((uint8_t)strtol(argv[3],NULL,0)); // year
		date_month_year[0] = DS_REG_DATE; // begin writing to calendar date register

		rc = cl_i2c_write_read(I2C_ADDRESS_DS3231, date_month_year, 4, NULL, 0); // write calendar registers
		if(rc) {
			printf("Error writing DS3231 calendar registers\n");
			return rc;
		}

//		// Clear OSF status register bit
//		uint8_t index_status[2] = {DS_REG_STATUS,0};
//		rc = cl_i2c_write_read(I2C_ADDRESS_DS3231, index_status, 2, NULL, 0); // write time registers
//		if(rc) {
//			printf("Error writing DS3231 status registers\n");
//			return rc;
//		}
		// Fall through - read registers and display time

	case 1:
		// No arguments - Display date - month/day/year

		// Check RTC, status register - OSF flag.  Flag should be clear
		//	rc = ds_time_valid();
		//	if(rc) return rc;

		// Read date, month, year into buffer
		rc = cl_i2c_write_read(I2C_ADDRESS_DS3231, &reg, 1, date_month_year, 3); // read [0]seconds, [1]minutes, [2]hours
		if(rc) {
			printf("Error reading DS3231 date, month, year registers\n");
			return rc;
		}
		printf("%d/%02d/20%02d\n",bcd_to_bin(date_month_year[1]),bcd_to_bin(date_month_year[0]),bcd_to_bin(date_month_year[2]));
		break;

	default:
		printf("Invalid number of arguments\n");
		return 1;
	} // switch(argc)

	return 0;
} // cl_ds_date


// Read the DS3231, return date and time via the DATE_TIME structure pointer
int read_rtc_into_date_time(DATE_TIME * dt)
{
	int rc;
	uint8_t reg=DS_REG_SECONDS;
	uint8_t rtc_buff[7];

	// Read time and calendar registers into buffer
	rc = cl_i2c_write_read(I2C_ADDRESS_DS3231, &reg, 1, rtc_buff, sizeof(rtc_buff));
	if(rc) {
		printf("Error reading DS3231 time calendar registers\n");
		return rc;
	}
	// The value ranges for each of the registers is the same as those defined for DATE_TIME.
	// No need to adjust values
	// Read, convert, write register values to DATE_TIME structure
	dt->seconds = bcd_to_bin(rtc_buff[0]);
	dt->minutes = bcd_to_bin(rtc_buff[1]);
	dt->hours   = bcd_to_bin(rtc_buff[2]);
	// not implementing day of the week
	dt->day     = bcd_to_bin(rtc_buff[4]);
	dt->month   = bcd_to_bin(rtc_buff[5]); // ignoring century bit
	dt->yOff    = bcd_to_bin(rtc_buff[6]);
	//printf("%s: %d/%02d/20%02d - %d:%02d:%02d\n",__func__,dt->month,dt->day,dt->yOff,dt->hours,dt->minutes,dt->seconds);
	return 0;
}

// Read / calculate Linux Timestamp / Epoch value and display it
// The value matches what linux command $ date +%s  displays
// If want time values to match, load RTC with UTC time, not local time
int cl_ds_time_stamp(void)
{
	// If DS3231 doesn't appear to be attached, return with error now
	int rc = ds3231_present(&hi2c1);
	if(HAL_OK != rc) return rc;

	DATE_TIME dt;
	read_rtc_into_date_time(&dt);

	uint32_t ts = unixtime(&dt);
	printf("TS: %lu\n",ts);
	return rc;
}



