#ifndef BRD_CONFIG_HG
#define BRD_CONFIG_HG

//***********************************************************************************
// Include files
//***********************************************************************************
/* System include statements */


/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_int.h"

/* The developer's include statements */


//***********************************************************************************
// defined files
//***********************************************************************************

// GPIO pin setup
#define STRONG_DRIVE

// LED 0 pin is
#define	LED0_PORT				gpioPortF
#define LED0_PIN				04u
#define LED0_DEFAULT			false 	// Default false (0) = off, true (1) = on
#define LED0_GPIOMODE			gpioModePushPull

// LED 1 pin is
#define LED1_PORT				gpioPortF
#define LED1_PIN				05u
#define LED1_DEFAULT			false	// Default false (0) = off, true (1) = on
#define LED1_GPIOMODE			gpioModePushPull

#ifdef STRONG_DRIVE
	#define LED0_DRIVE_STRENGTH		gpioDriveStrengthStrongAlternateStrong
	#define LED1_DRIVE_STRENGTH		gpioDriveStrengthStrongAlternateStrong
#else
	#define LED0_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak
	#define LED1_DRIVE_STRENGTH		gpioDriveStrengthWeakAlternateWeak

#endif


// System Clock setup
#define MCU_HFXO_FREQ			cmuHFRCOFreq_32M0Hz //1MOHz -> 32MOHz


// LETIMER PWM Configuration
#define		PWM_ROUTE_0			LETIMER_ROUTELOC0_OUT0LOC_LOC28
#define		PWM_ROUTE_1			LETIMER_ROUTELOC0_OUT1LOC_LOC28

// Si7021 Temperature Sensor Pin Configuration
#define	SI7021_SCL_PORT				gpioPortC
#define	SI7021_SCL_PIN				11u
#define SI7021_SCL_DEFAULT			true // When no activity no Bus, SCL/SDA high (pulled up to Vcc)
#define SI7021_SCL_GPIOMODE			gpioModeWiredAnd

#define	SI7021_SDA_PORT				gpioPortC
#define	SI7021_SDA_PIN				10u
#define SI7021_SDA_DEFAULT			true
#define SI7021_SDA_GPIOMODE			gpioModeWiredAnd

#define	SI7021_SENSOR_EN_PORT		gpioPortB
#define	SI7021_SENSOR_EN_PIN		10u
#define SI7021_SENSOR_EN_DEFAULT 	true
#define SI7021_SENSOR_EN_GPIOMODE	gpioModePushPull
#define SI7021_SENSOR_EN_STRENGTH	gpioDriveStrengthWeakAlternateWeak

// I2C Configuration
#define		I2C_SCL_ROUTE				I2C_ROUTELOC0_SCLLOC_LOC15
#define		I2C_SDA_ROUTE				I2C_ROUTELOC0_SDALOC_LOC15

// UART Configuration
#define		LEUART_TX_PORT			gpioPortD
#define		LEUART_TX_PIN			10
#define		LEUART_TX_DEFAULT		true
#define		LEUART_TX_GPIOMODE		gpioModePushPull
#define		LEUART_TX_STRENGTH		gpioDriveStrengthStrongAlternateWeak


#define		LEUART_RX_PORT			gpioPortD
#define		LEUART_RX_PIN			11
#define		LEUART_RX_DEFAULT		true
#define		LEUART_RX_GPIOMODE		gpioModeInput
// There's no need to set the drive strength of an input pin

#define LEUART0_TX_ROUTE				LEUART_ROUTELOC0_TXLOC_LOC18   	// Route to Loc18
#define LEUART0_RX_ROUTE				LEUART_ROUTELOC0_RXLOC_LOC18   	// Route to Loc18


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************

#endif
