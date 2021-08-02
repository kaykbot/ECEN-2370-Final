/**
 * @file app.c
 * @author Keith Graham
 * @modified Kay Sho
 * @9/12/2020
 *
 * @brief Contains all app driver functions
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"



//***********************************************************************************
// defined files
//***********************************************************************************
//#define BLE_TEST_ENABLED
#define CIRC_BUFF_TEST_ENABLED
//***********************************************************************************
// Static / Private Variables
//***********************************************************************************
static char receive_str[50];
static bool setting;
//***********************************************************************************
// Private functions
//***********************************************************************************

static void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route);

//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * Sets up the peripherals and the call backs functions
 *
 * @details
 * Setting up the scheduled call backs involves checking if they are true, and then
 * removing them.
 *
 * @note
 *
 *
 ******************************************************************************/
void app_peripheral_setup(void){
	cmu_open();
	gpio_open();
	app_letimer_pwm_open(PWM_PER, PWM_ACT_PER, PWM_ROUTE_0, PWM_ROUTE_1);
	scheduler_open();
	sleep_open();
	sleep_block_mode(SYSTEM_BLOCK_EM);
	add_scheduled_event(BOOT_UP_CB);
	si7021_i2c_open();
	ble_open(BLE_TX_DONE_CB, BLE_RX_DONE_CB);

}

/***************************************************************************//**
 * @brief
 * Contains the letimer0 COMP0 callback event
 *
 * @details
 *
 * When COMP0 is detected, the following code will run, and the
 * event is removed.
 *
 * @note
 *
 *
 ******************************************************************************/
void scheduled_letimer0_comp0_cb(void){
	EFM_ASSERT(false);
	remove_scheduled_event(LETIMER0_COMP0_CB);
}

/***************************************************************************//**
 * @brief
 * Contains the letimer0 COMP1 callback event
 *
 * @details
 *
 * When COMP1 is detected, the following code will run, and the
 * event is removed.
 *
 * @note
 *
 *
 ******************************************************************************/
void scheduled_letimer0_comp1_cb(void){
	EFM_ASSERT(false);
	remove_scheduled_event(LETIMER0_COMP1_CB);
}

/***************************************************************************//**
 * @brief
 * Contains the letimer0 underflow callback event
 *
 * @details
 *
 * When the underflow event is detected, the event is removed and reset, and the
 * rest of the code is run.
 *
 * @note
 *
 *
 *
 ******************************************************************************/
void scheduled_letimer0_uf_cb(void){
	EFM_ASSERT(get_scheduled_events() & LETIMER0_UF_CB);
	remove_scheduled_event(LETIMER0_UF_CB);
	si7021_read(SI7021_READ_DONE_CB);
}

/***************************************************************************//**
 * @brief
 * Contains the completion event for Boot up event
 *
 * @details
 * This is where we test the BLE device
 *
 * @note
 * This is the best place to write whatever you'd like.
 *
 ******************************************************************************/
void scheduled_boot_up_cb(void){
	EFM_ASSERT(get_scheduled_events() & BOOT_UP_CB); // the EFM_ASSERT is NOT an example of TDD
	remove_scheduled_event(BOOT_UP_CB);

#ifdef BLE_TEST_ENABLED
	bool test = ble_test("KaySho");
	EFM_ASSERT(test);
	timer_delay(2000);
#endif
#ifdef CIRC_BUFF_TEST_ENABLED
	circular_buff_test();
#endif
	ble_write("\nHello World!\nKay Sho\n\0");
	ble_write("\nPlease use She or They to \nrefer to them!\n\0");
	ble_write("\nShe would like to thank you \nfor the wonderful course!\n\0");
}

/***************************************************************************//**
 * @brief
 * Contains the completion event for TX complete event
 *
 * @details
 *	Handles the completion event for the TX data
 *
 * @note
 *	We've moved the letimer_start(LETIMER0, true) down here
 *
 ******************************************************************************/
void scheduled_tx_done_cb(void){
	EFM_ASSERT(get_scheduled_events()& BLE_TX_DONE_CB);
	remove_scheduled_event(BLE_TX_DONE_CB);
	ble_circ_pop(false);
	letimer_start(LETIMER0, true); //start LETIMER0 when TX complete
}
/***************************************************************************//**
 * @brief scheduled_rx_done_cb()
 * Contains the completion event for RX complete event
 *
 * @details
 *	Handles the completion event for the RX data event
 *
 * @note
 *	IMPERIAL is set to true, requiring conversion
 *	METRIC is set to false, meaning no conversion
 *	For more detail on how this is done, see the bottom of the Si7021.c file.
 ******************************************************************************/
void scheduled_rx_done_cb(void){
	EFM_ASSERT(get_scheduled_events()& BLE_RX_DONE_CB);
	remove_scheduled_event(BLE_RX_DONE_CB);
	received_data(receive_str);

	if(!strcmp(receive_str, "#tempf!")){
		setting = IMPERIAL;
		ble_write("\nTemperature converting to F\n");
		return;
	}
	else if(!strcmp(receive_str, "#tempc!")){
		setting = METRIC;
		ble_write("\nTemperature converting to C\n");
		return;
	}
	else ble_write("\nUnknown Command\n");
}
/***************************************************************************//**
 * @brief
 * Contains the completion event for the Si7021 onboard temperature sensor.
 *
 * @details
 *
 * When the Si7021 finishes reading the temperature, the Si7021 will take the
 * converted temperature and checks whether or not it is high enough to meet condition.
 *
 * @note
 * METRIC == false, meaning there is no conversion
 * IMPERIAL == true, meaning there is conversion from C to F
 * Currently, Threshold temperature is set to 80.6 F, or 27.0 C.
 *
 ******************************************************************************/
void scheduled_si7021_read_done_cb(void){
	EFM_ASSERT(get_scheduled_events() & SI7021_READ_DONE_CB);
	remove_scheduled_event(SI7021_READ_DONE_CB);
	//METRIC CONVERSION
	if(!setting){
		float tempC = si7021_temp_met();
		if(tempC >= 27.0){GPIO_PinOutSet(LED1_PORT, LED1_PIN);}
		else{GPIO_PinOutClear(LED1_PORT, LED1_PIN);}
		sprintf(buffer, "Temp = %d.%d C\n", (int)tempC, (int)(tempC*10)%10);
	}
	//IMPERIAL CONVERSION
	else if(setting){
		float tempF = si7021_temp_imp();
		if(tempF >= 80.6){GPIO_PinOutSet(LED1_PORT, LED1_PIN);}
		else{GPIO_PinOutClear(LED1_PORT, LED1_PIN);}
		sprintf(buffer, "Temp = %d.%d F\n", (int)tempF, (int)(tempF*10)%10);
	}
	ble_write(buffer);
}
/***************************************************************************//**
 * @brief
 * Contains the APP LETIMER PWM structs and calls
 *
 * @details
 * Setting up the pwm struct for interrupts makes things far less complicated
 *
 *
 * @param[in] period
 *	Floating point number that defines the full period that the device's PWM runs on.
 * @param[in] act_period
 *  Floating point number that defines the active period of the device's PWM.
 * @param[in] out0_route
 * 	Unsigned 32-bit integer that defines the output of the route0 pin.
 * @param[in] out0_route
 * 	Unsigned 32-bit integer that defines the output of the route01 pin.
 ******************************************************************************/
void app_letimer_pwm_open(float period, float act_period, uint32_t out0_route, uint32_t out1_route){
	// Initializing LETIMER0 for PWM operation by creating the
	// letimer_pwm_struct and initializing all of its elements
	APP_LETIMER_PWM_TypeDef app_letimer_pwm_struct;

	// Declaration of the app_letimer_PWM struct elements
	app_letimer_pwm_struct.debugRun			= false;
	app_letimer_pwm_struct.enable			= false;
	app_letimer_pwm_struct.out_pin_0_en 	= false;
	app_letimer_pwm_struct.out_pin_1_en 	= false;
	app_letimer_pwm_struct.out_pin_route0 	= out0_route; 	//expects uint32_bit arg; numeral arg
	app_letimer_pwm_struct.out_pin_route1 	= out1_route;
	app_letimer_pwm_struct.period 			= period;
	app_letimer_pwm_struct.active_period 	= act_period;

	app_letimer_pwm_struct.comp0_irq_enable = false;
	app_letimer_pwm_struct.comp0_cb			= LETIMER0_COMP0_CB;
	app_letimer_pwm_struct.comp1_irq_enable = false;
	app_letimer_pwm_struct.comp1_cb			= LETIMER0_COMP1_CB;
	app_letimer_pwm_struct.uf_irq_enable	= true;
	app_letimer_pwm_struct.uf_cb			= LETIMER0_UF_CB;

	letimer_pwm_open(LETIMER0, &app_letimer_pwm_struct);

}


