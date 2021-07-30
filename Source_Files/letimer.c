 /**
 * @file letimer.c
 * @author Kay Sho
 * @date January 12th, 2020
 * @brief Contains all the LETIMER driver functions
 *
 */


//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Libraries

//** Silicon Lab include files

//** User/developer include files
#include "letimer.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Private variables
//***********************************************************************************
static uint32_t scheduled_comp0_cb;
static uint32_t scheduled_comp1_cb;
static uint32_t scheduled_uf_cb;
//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   Driver to open an set an LETIMER peripheral in PWM mode
 *
 * @details
 * 	 This routine is a low level driver.  The application code calls this function
 * 	 to open one of the LETIMER peripherals for PWM operation to directly drive
 * 	 GPIO output pins of the device and/or create interrupts that can be used as
 * 	 a system "heart beat" or by a scheduler to determine whether any system
 * 	 functions need to be serviced.
 *
 * @note
 *   This function is normally called once to initialize the peripheral and the
 *   function letimer_start() is called to turn-on or turn-off the LETIMER PWM
 *   operation.
 *
 * @param[in] letimer
 *   Pointer to the base peripheral address of the LETIMER peripheral being opened
 *
 * @param[in] app_letimer_struct
 *   Is the STRUCT that the calling routine will use to set the parameters for PWM
 *   operation
 *
 ******************************************************************************/
void letimer_pwm_open(LETIMER_TypeDef *letimer, APP_LETIMER_PWM_TypeDef *app_letimer_pwm_struct){
	LETIMER_Init_TypeDef letimer_pwm_values;

	unsigned int period_cnt;
	unsigned int period_active_cnt;

	/*  Initializing LETIMER for PWM mode */
	/*  Enable the routed clock to the LETIMER0 peripheral */
	if (letimer == LETIMER0)
	CMU_ClockEnable(cmuClock_LETIMER0, true);

	letimer_start(letimer, false);

	letimer->CMD = LETIMER_CMD_START;
	while (letimer->SYNCBUSY);
	EFM_ASSERT(letimer->STATUS & LETIMER_STATUS_RUNNING);
	letimer->CMD = LETIMER_CMD_STOP;
	while(letimer->SYNCBUSY);
	letimer->CNT = 0;

	letimer_pwm_values.bufTop 	= true;								// Comp1 will not be used to load comp0, but used to create an on-time/duty cycle
	letimer_pwm_values.comp0Top = true;								// load comp0 into cnt register when count register underflows enabling continuous looping
	letimer_pwm_values.debugRun = app_letimer_pwm_struct->debugRun;
	letimer_pwm_values.enable 	= app_letimer_pwm_struct->enable;	// Don't want to usually have this on
	letimer_pwm_values.out0Pol 	= 0;								// While PWM is not active out, idle is DEASSERTED, 0
	letimer_pwm_values.out1Pol 	= 0;								// While PWM is not active out, idle is DEASSERTED, 0
	letimer_pwm_values.repMode 	= letimerRepeatFree;				// Setup letimer for free running for continuous looping
	letimer_pwm_values.ufoa0 	= letimerUFOAPwm ;					// Using the HAL documentation, set to PWM mode
	letimer_pwm_values.ufoa1 	= letimerUFOAPwm ;					// Using the HAL documentation, set to PWM mode

	LETIMER_Init(letimer, &letimer_pwm_values);		// Initialize letimer
	while(letimer->SYNCBUSY);

	period_cnt = app_letimer_pwm_struct->period * LETIMER_HZ;
	letimer->COMP0 = period_cnt;
	period_active_cnt = app_letimer_pwm_struct->active_period * LETIMER_HZ;
	letimer->COMP1 = period_active_cnt;

	letimer->REP0 = 1;
	letimer->REP1 = 1;

	LETIMER0->ROUTELOC0 = app_letimer_pwm_struct -> out_pin_route0;
	LETIMER0->ROUTELOC0 |=  app_letimer_pwm_struct -> out_pin_route1;

	if (app_letimer_pwm_struct->out_pin_0_en)
	{letimer->ROUTEPEN = LETIMER_ROUTEPEN_OUT0PEN;}
	if (app_letimer_pwm_struct->out_pin_1_en)
	{letimer->ROUTEPEN |= LETIMER_ROUTEPEN_OUT1PEN;}
	/* We are now enabling the interrupts set via the APP_LETIMER_PWM_TypeDef input argument.
	 * We should use the IEN bit
	 */
	scheduled_comp0_cb = app_letimer_pwm_struct->comp0_cb;
	scheduled_comp1_cb = app_letimer_pwm_struct->comp1_cb;
	scheduled_uf_cb = app_letimer_pwm_struct->uf_cb;

	letimer->IFC = LETIMER_CLEAR;

	if(app_letimer_pwm_struct->comp0_irq_enable)
	{LETIMER_IntEnable(letimer, LETIMER_IEN_COMP0);}

	if(app_letimer_pwm_struct->comp1_irq_enable)
	{LETIMER_IntEnable(letimer, LETIMER_IEN_COMP1);}

	if(app_letimer_pwm_struct->uf_irq_enable)
	{LETIMER_IntEnable(letimer, LETIMER_IEN_UF);}

	NVIC_EnableIRQ(LETIMER0_IRQn);

	if(letimer->STATUS & LETIMER_STATUS_RUNNING){ //Checks if running
	sleep_block_mode(LETIMER_EM);}

}
/***************************************************************************//**
 * @brief
 * 		Starts selected low energy timer.
 * @details
 * 		Checks the letimer status, if it is running, and if it's enabled, and acts
 * 		accordingly.
 * @note
 *
 * @param[in] letimer
 * 		Allows low energy timer modularity.
 * @param[in] enable
 * 		Selects on or off.
 ******************************************************************************/

void letimer_start(LETIMER_TypeDef *letimer, bool enable){

	if (!(letimer->STATUS & LETIMER_STATUS_RUNNING) && enable){
		sleep_block_mode(LETIMER_EM);}

	if ((letimer->STATUS & LETIMER_STATUS_RUNNING) && !enable){
		sleep_unblock_mode(LETIMER_EM);}

	LETIMER_Enable(letimer, true);

	while (letimer->SYNCBUSY);
}
/***************************************************************************//**
 * @brief LETIMER_IRQHandler(void)
 * 		Sets up LETIMER ISR
 * @details
 * 		Clears interrupt flag, and then checks each callback mode so that it may
 * 		activate checked callback mode
 * @note
 *
 * @param[in]
 *
 *
 ******************************************************************************/

void LETIMER0_IRQHandler(void){
	uint32_t int_flag;
	int_flag = LETIMER0->IF & LETIMER0->IEN; 	// Reads
	LETIMER0->IFC = int_flag;					// Clear

	if(int_flag & LETIMER_IF_COMP0)
	{
		EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_COMP0));
		add_scheduled_event(scheduled_comp0_cb);
	}
	if(int_flag & LETIMER_IF_COMP1)
	{
		EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_COMP1));
		add_scheduled_event(scheduled_comp1_cb);
	}
	if(int_flag & LETIMER_IF_UF)				// Just check the underflow flag and bit
	{
		EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_UF));
		add_scheduled_event(scheduled_uf_cb);
	}
}
