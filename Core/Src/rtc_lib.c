/*
 * rtc_lib.c
 *
 * Some functions taken from AdaFruit's RTClib.cpp / RTClib.h - converted to "C"
 */
#include <stdint.h> // uint8_t
#include "rtc_lib.h"


/**
  Number of days in each month, from January to November. December is not
  needed. Omitting it avoids an incompatibility with Paul Stoffregen's Time
  library. C.f. https://github.com/adafruit/RTClib/issues/114
*/
const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30}; // 11 months (intentional)

/**************************************************************************/
/*!
    @brief  Given a date, return number of days since 2000/01/01,
            valid for 2000--2099
    @param y Year
    @param m Month
    @param d Day
    @return Number of days
*/
/**************************************************************************/
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d)
{
  if (y >= 2000U)
    y -= 2000U;
  uint16_t days = d;
  for (uint8_t i = 1; i < m; ++i)
    days += daysInMonth[i - 1];
  if (m > 2 && y % 4 == 0)
    ++days;
  return days + 365 * y + (y + 3) / 4 - 1;
}

/**************************************************************************/
/*!
    @brief  Given a number of days, hours, minutes, and seconds, return the
   total seconds
    @param days Days
    @param h Hours
    @param m Minutes
    @param s Seconds
    @return Number of seconds total
*/
/**************************************************************************/
static uint32_t time2ulong(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
  return ((days * 24UL + h) * 60 + m) * 60 + s;
}

/**************************************************************************/
/*!
  @brief  Return Unix time: seconds since 1 Jan 1970.

  @see The `DateTime::DateTime(uint32_t)` constructor is the converse of
	  this method.

  @return Number of seconds since 1970-01-01 00:00:00.
*/
/**************************************************************************/
uint32_t unixtime(DATE_TIME * dt)
{
	uint32_t t;
	uint16_t days = date2days(dt->yOff, dt->month, dt->day);
	t = time2ulong(days, dt->hours, dt->minutes, dt->seconds);
	t += SECONDS_FROM_1970_TO_2000; // seconds from 1970 to 2000

	return t;
}


