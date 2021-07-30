/*
 * si7021.h
 *
 *  Created on: Oct 4, 2020
 *      Author: Kay Sho
 */

#ifndef SI7021_HG
#define SI7021_HG

/* System include statements */


/* Silicon Labs include statements */
#include "i2c.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define SI7021_ADDR				0x40
#define SI7021_TEMP_NO_HOLD		0xF3
#define SI7021_I2C				I2C0

#define SI7021_REF_FREQ						0
#define SI7021_NUM_BYTES_TEMP_CHECKSUM		6
#define SI7021_NUM_BYTES_TEMP_NOCHECKSUM	2
//***********************************************************************************
// function prototypes
//***********************************************************************************

void si7021_i2c_open(void);
void si7021_read(uint32_t event);
float si7021_temp_met();
float si7021_temp_imp();

#endif /* SI7021_HG */
