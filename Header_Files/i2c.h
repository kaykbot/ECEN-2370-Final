/*
 * i2c.h
 *
 *  Created on: Oct 1, 2020
 *      Author: Kay Sho
 *      Pronouns: (She/They)
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef I2C_HG
#define I2C_HG

/* System include statements */
#include <stdint.h>

/* Silicon Labs include statements */
#include <em_emu.h>
#include "em_int.h"
#include "em_core.h"
#include "em_assert.h"
#include "em_i2c.h"
#include "em_cmu.h"
/* The developer's include statements */
#include "sleep_routines.h"
#include "brd_config.h"
#include "scheduler.h"
#include "gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************

#define I2C_EM_BLOCK	EM2 //Prevents entrance into EM2
#define I2C_WRITE		0
#define I2C_READ		1

#define RESET_TOGGLE_NUMBER		18


//***********************************************************************************
// global variables
//***********************************************************************************

typedef struct{
	bool					enable;
	bool					master;
	uint32_t				refFreq;
	uint32_t				freq;
	I2C_ClockHLR_TypeDef	clhr;

//	Pin setup

	uint32_t				scl_pin_route;		// out 0 route to gpio port/pin
	uint32_t				sda_pin_route;		// out 1 route to gpio port/pin
	bool					scl_pin_en;			// enable out 0 route
	bool					sda_pin_en;			// enable out 1 route

//	I/O Pins (for use in the si7021

	uint32_t				scl_pin;			// Port
	uint32_t				sda_pin;			// Pin
	uint32_t				scl_port;
	uint32_t				sda_port;
}I2C_OPEN_STRUCT;


//***********************************************************************************
// function prototypes
//***********************************************************************************
void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *i2c_init);
void i2c_start(I2C_TypeDef *i2c, uint8_t dev_addr, bool rw_mode, uint8_t reg_addr, uint32_t* sm_data, uint8_t sm_data_len, uint32_t event, bool enable);
//uint32_t *data


#endif /* I2C_HG */
