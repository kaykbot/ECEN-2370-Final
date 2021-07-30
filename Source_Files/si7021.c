/**
 * @file si7021.c
 * @author Kay Sho
 * @date  10/4/2020
 * @brief Contains the driver function for the si7021 Temperature and Humidity sensor
 * @note The author's pronouns: (She/They)
 */


//***********************************************************************************
// Include files
//***********************************************************************************

/* System include statements */


/* Silicon Labs include statements */


/* The developer's include statements */
#include "si7021.h"



//***********************************************************************************
// defined files
//***********************************************************************************

//***********************************************************************************
// private variables
//***********************************************************************************
static uint32_t sidata[SI7021_NUM_BYTES_TEMP_CHECKSUM]; //Created mem location
//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * Contains the si7021 function that uses and calls the functions defined in i2c.c/.h
 *
 * @details
 * Uses the functions defined in i2c.c source files as well as the structs in i2c.h to
 * run the Si7021 On-board temperature sensor.
 *
 *
 * @note
 *
 *
 ******************************************************************************/
void si7021_i2c_open(void){

	I2C_OPEN_STRUCT Si7021_I2C_val;


	//	Si7021 bus initialization
	Si7021_I2C_val.enable 			= true;
	Si7021_I2C_val.master 			= true;
	Si7021_I2C_val.refFreq 			= SI7021_REF_FREQ;
	Si7021_I2C_val.freq 			= I2C_FREQ_FAST_MAX;
	Si7021_I2C_val.clhr 			= i2cClockHLRAsymetric;

	//	Si7021 Pin struct set up
	Si7021_I2C_val.scl_pin_en		= true;
	Si7021_I2C_val.scl_pin_route	= I2C_SCL_ROUTE;
	Si7021_I2C_val.sda_pin_en		= true;
	Si7021_I2C_val.sda_pin_route	= I2C_SDA_ROUTE;


	i2c_open(I2C0, &Si7021_I2C_val); // Must fix for modularity & encapsulation
}
/***************************************************************************//**
 * @brief
 *	Si7021 temperature read function
 *
 * @details
 *	Runs the Si7021 temperature read function. This is done in No Hold [Manager] Mode
 *
 * @note
 *
 * @param[in] event
 *	Scheduler event associated with the Si7021 Temperature measurement
 *
 *
 ******************************************************************************/
void si7021_read(uint32_t event){
	i2c_start(SI7021_I2C, SI7021_ADDR, I2C_READ, SI7021_TEMP_NO_HOLD, sidata, SI7021_NUM_BYTES_TEMP_CHECKSUM, event, true);}

/***************************************************************************//**
 * @brief
 * Contains si7021 Celcius temperature conversion function
 *
 * @details
 *
 * Takes detected temperature from the sidata array, originally in Centigrade, and keeps it as
 * is.
 *

 ******************************************************************************/

float si7021_temp_met(){
	uint16_t temp_dat = (sidata[0] << 8) | sidata[1];
	float temp_c = ((float)175.72*(float)temp_dat / (float)65536) - (float)46.85;
	return temp_c;
}

/***************************************************************************//**
 * @brief
 * Contains si7021 Fahrenheit temperature conversion function
 *
 * @details
 *
 * Takes detected temperature from the sidata array, originally in Centigrade, and converts
 * the temperature into Fahrenheit/Imperial units.
 *
 * @note
 *
 * Fact: Metric will always be superior to Imperial. (this is the author's very serious opinion)
 *
 * The resolution is x bits
 *
 ******************************************************************************/

float si7021_temp_imp(){
	uint16_t temp_dat = (sidata[0] << 8) | sidata[1];
	float temp_c = ((float)175.72*(float)temp_dat / (float)65536) - (float)46.85;
	return (temp_c * (float)1.8 + (float)32);
}
