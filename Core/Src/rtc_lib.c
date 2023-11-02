/*
 * rtc_lib.c
 *
 * Some functions taken from AdaFruit's RTClib.cpp / RTClib.h - converted to "C"
 */
#include <stdint.h> // uint8_t
#include "rtc_lib.h"


// What is epoch time?  (From epochconvert.com)
// The Unix epoch (or Unix time or POSIX time or Unix timestamp) is the number of seconds
// that have elapsed since January 1, 1970 (midnight UTC/GMT), not counting leap seconds
// (in ISO 8601: 1970-01-01T00:00:00Z). Literally speaking the epoch is Unix time 0,
// (midnight 1/1/1970), but 'epoch' is often used as a synonym for Unix time. Some systems
// store epoch dates as a signed 32-bit integer, which might cause problems on
// January 19, 2038 (known as the Year 2038 problem or Y2038).

/**
  Number of days in each month, from January to November. December is not
  needed. Omitting it avoids an incompatibility with Paul Stoffregen's Time
  library. C.f. https://github.com/adafruit/RTClib/issues/114
*/
const uint8_t daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
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
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; i++)
        days += daysInMonth[i - 1];
    if (m > 2 && y % 4 == 0)
        days++;
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
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

/**************************************************************************/
/*!
    @brief  Constructor from
        [Unix time](https://en.wikipedia.org/wiki/Unix_time).

    Fill a DATE_TIME from an unsigned integer specifying the number of seconds
    elapsed since the epoch: 1970-01-01 00:00:00. This number is analogous
    to Unix time, with two small differences:

     - The Unix epoch is specified to be at 00:00:00
       [UTC](https://en.wikipedia.org/wiki/Coordinated_Universal_Time),
       whereas this class has no notion of time zones. The epoch used in
       this class is then at 00:00:00 on whatever time zone the user chooses
       to use, ignoring changes in DST.

     - Unix time is conventionally represented with signed numbers, whereas
       this constructor takes an unsigned argument. Because of this, it does
       _not_ suffer from the
       [year 2038 problem](https://en.wikipedia.org/wiki/Year_2038_problem).

    @see The `unixtime()` method is the converse of this constructor.

    @param t Time elapsed in seconds since 1970-01-01 00:00:00.
*/
/**************************************************************************/
void unix_to_date_time(DATE_TIME * dt, uint32_t t)
{
  t -= SECONDS_FROM_1970_TO_2000; // bring to 2000 timestamp from 1970

  dt->seconds = t % 60;
  t /= 60;
  dt->minutes = t % 60;
  t /= 60;
  dt->hours = t % 24;
  uint16_t days = t / 24;
  uint8_t leap;
  for (dt->yOff = 0;; dt->yOff++) {
    leap = dt->yOff % 4 == 0;
    if (days < 365U + leap)
      break;
    days -= 365 + leap;
  }
  for (dt->month = 1; dt->month < 12; dt->month++) {
    uint8_t daysPerMonth = daysInMonth[dt->month - 1];
    if (leap && dt->month == 2)
      ++daysPerMonth;
    if (days < daysPerMonth)
      break;
    days -= daysPerMonth;
  }
  dt->day = days + 1;
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
	uint16_t days = date2days(dt->yOff, dt->month, dt->day);
	uint32_t t = time2ulong(days, dt->hours, dt->minutes, dt->seconds);
	t += SECONDS_FROM_1970_TO_2000; // seconds from 1970 to 2000

	return t;
}


