// Copyright Jim Merkle, 11/01/2023
// File: cl_ds3231.c
//
// Implement various command line routines that interact with the DS3231 RTC
//
#include "command_line.h"
#include "cl_i2c.h"
#include "cl_ds3231.h"


/* To implement the expected functionality, the following lines would be added to
   command_line.c, in the cmd_table[]:

#ifdef HAL_I2C_MODULE_ENABLED
    {"time",      "time <hrs:min:sec>",                           1, cl_ds_time},
#endif // HAL_I2C_MODULE_ENABLED
*/

