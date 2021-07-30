//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

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

void cmu_open(void){

		CMU_ClockEnable(cmuClock_HFPER, true);

		// By default, Low Frequency Resistor Capacitor Oscillator, LFRCO, is enabled,
		// Disable the LFRCO oscillator cmuOsc_LFRCO
		CMU_OscillatorEnable(cmuOsc_LFRCO , false, false);	 // What is the enumeration required for LFRCO?

		// Disable the Low Frequency Crystal Oscillator, cmuOsc_LFXO
		CMU_OscillatorEnable(cmuOsc_LFXO , true, true);	// What is the enumeration required for LFXO?
		// wait = true b/c it takes some time to enable the clock

		// No requirement to enable the ULFRCO oscillator.  It is always enabled in EM0-4H

		//Route LF (cmuClock_LFB) clock to LEUART
		CMU_ClockSelectSet(cmuClock_LFB , cmuSelect_LFXO);

		// Route LF (cmuClock_LFA) clock to LETIMER0 (prefix: cmuSelect) clock tree
		CMU_ClockSelectSet(cmuClock_LFA , cmuSelect_ULFRCO);

		// Now, you must ensure that the global Low Frequency is enabled
		CMU_ClockEnable(cmuClock_CORELE , true);	//This enumeration is found in the Lab 2 assignment
		// The cmuClock_HFLE is the same item as cmuClock_CORELE
}

