//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"


//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************


//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

void gpio_open(void){

	CMU_ClockEnable(cmuClock_GPIO, true);

	// Configure LED pins
	GPIO_DriveStrengthSet(LED0_PORT, LED0_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED0_PORT, LED0_PIN, LED0_GPIOMODE, LED0_DEFAULT);

	GPIO_DriveStrengthSet(LED1_PORT, LED1_DRIVE_STRENGTH);
	GPIO_PinModeSet(LED1_PORT, LED1_PIN, LED1_GPIOMODE, LED1_DEFAULT);

	// Configure Si7021 Pins
	GPIO_DriveStrengthSet(SI7021_SENSOR_EN_PORT, SI7021_SENSOR_EN_STRENGTH);
	GPIO_PinModeSet(SI7021_SENSOR_EN_PORT, SI7021_SENSOR_EN_PIN, SI7021_SENSOR_EN_GPIOMODE, SI7021_SENSOR_EN_DEFAULT);
	GPIO_PinModeSet(SI7021_SCL_PORT, SI7021_SCL_PIN, SI7021_SCL_GPIOMODE, SI7021_SCL_DEFAULT);
	GPIO_PinModeSet(SI7021_SDA_PORT, SI7021_SDA_PIN, SI7021_SDA_GPIOMODE, SI7021_SDA_DEFAULT);

	// Configure UART Pins - TX
	GPIO_DriveStrengthSet(LEUART_TX_PORT, LEUART_TX_STRENGTH);
	GPIO_PinModeSet(LEUART_TX_PORT, LEUART_TX_PIN, LEUART_TX_GPIOMODE, LEUART_TX_DEFAULT);

	// Configure UART Pins - RX
	GPIO_PinModeSet(LEUART_RX_PORT, LEUART_RX_PIN, LEUART_RX_GPIOMODE, LEUART_TX_DEFAULT);
}
