//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	LETIMER_HG
#define	LETIMER_HG

/* System include statements */


/* Silicon Labs include statements */
#include "em_letimer.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_assert.h"

/* The developer's include statements */
#include "scheduler.h"


//***********************************************************************************
// defined files
//***********************************************************************************
#define LETIMER_HZ		1000			// Utilizing ULFRCO oscillator for LETIMERs
#define LETIMER_EM		EM4				// Usuing the ULFRCO, block from entering EM4
#define LETIMER_CLEAR	0b11111			// clear interrupt flag
//***********************************************************************************
// global variables
//***********************************************************************************
typedef struct {
	bool 			debugRun;			// True = keep LETIMER running will halted
	bool 			enable;				// enable the LETIMER upon completion of open
	uint32_t		out_pin_route0;		// out 0 route to gpio port/pin
	uint32_t		out_pin_route1;		// out 1 route to gpio port/pin
	bool			out_pin_0_en;		// enable out 0 route
	bool			out_pin_1_en;		// enable out 1 route
	float			period;				// seconds
	float			active_period;		// seconds

	// Bottom 6 are new elements from Lab 3 dealing with interrupts
	bool			comp0_irq_enable;	// enable interrupt on comp0 interrupt
	uint32_t		comp0_cb;			// comp0 CallBack
	bool			comp1_irq_enable;	// enable interrupt on comp1 interrupt
	uint32_t		comp1_cb;			// comp1 CallBack
	bool			uf_irq_enable;		// enable interrupt on Underflow interrupt
	uint32_t		uf_cb;				// underflow CallBack
} APP_LETIMER_PWM_TypeDef ;


//***********************************************************************************
// function prototypes
//***********************************************************************************
void letimer_pwm_open(LETIMER_TypeDef *letimer, APP_LETIMER_PWM_TypeDef *app_letimer_pwm_struct);
void letimer_start(LETIMER_TypeDef *letimer, bool enable);
void LETIMER0_IRQHandler(void);

#endif
