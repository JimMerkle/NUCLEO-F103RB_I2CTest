/*
 * rtc_lib.h
 *
 * Some functions taken from AdaFruit's RTClib.cpp / RTClib.h
 */

#ifndef _RTC_LIB_H_
#define _RTC_LIB_H_

// Constants
#define SECONDS_PER_DAY 86400L ///< 60 * 60 * 24
#define SECONDS_FROM_1970_TO_2000                                              \
  946684800 ///< Unixtime for 2000-01-01 00:00:00, useful for initialization

// Data structures
typedef struct {
	uint8_t yOff;    // year offset from year 2000
	uint8_t month;   // 1-12
	uint8_t day;     // calendar date 1-31 (not day of the week)
	uint8_t hours;   // 0-23, 0 == midnight
	uint8_t minutes; // 0-59
	uint8_t seconds; // 0-59
} DATE_TIME;

/**************************************************************************/
/*!
  @brief  Return Unix time: seconds since 1 Jan 1970.
*/
/**************************************************************************/
uint32_t unixtime(DATE_TIME * dt);

#endif // _RTC_LIB_H_
