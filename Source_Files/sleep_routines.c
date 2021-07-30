/**
 * @file si7021.c
 * @author Kay Sho
 * @date  10/4/2020
 * @brief Contains the driver functions to handle the sleep routines and energy modes.
 * @note The author's pronouns: (She/They)
 */
//***********************************************************************************
// Include files
//***********************************************************************************

#include "sleep_routines.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// Static / Private Variables
//***********************************************************************************
static int lowest_energy_mode[MAX_EM];

//***********************************************************************************
// Private functions
//***********************************************************************************


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * States all of the functions to manage the sleep modes/energy modes
 *
 * @details
 * Sets up the functions defined in sleep_routines.h such that the proper
 * energy mode can be set up.
 *
 * This depends on what conditions lowest_energy_mode finds.
 *
 * @note
 *
 *
 ******************************************************************************/
void sleep_open(void){
	int i;
	for (i=0;i< MAX_EM; i++){
		CORE_DECLARE_IRQ_STATE;
		CORE_ENTER_CRITICAL();
		lowest_energy_mode[i] = 0;
		CORE_EXIT_CRITICAL();
	}
}
/***************************************************************************//**
 * @brief sleep_block_mode(uint32_t EM)
 * Prevent entrance into sleep mode while peripheral active
 * @details
 * Checks if EM is less than 5, then increments to Max energy mode
 * @note
 *
 * @param[in] EM
 * Energy Mode
 * @param[in]
 *
 ******************************************************************************/

void sleep_block_mode(uint32_t EM){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	EFM_ASSERT(lowest_energy_mode[EM] < 5);

		lowest_energy_mode[EM]++;

	CORE_EXIT_CRITICAL();
}
/***************************************************************************//**
 * @brief sleep_unblock_mode(uint32_t EM)
 * Allows entrance into sleep mode
 * @details
 * Lowers energy mode array to lowest of 0, so that all peripherals can activate
 * @note
 *
 * @param[in] EM
 * Energy Mode
 * @param[in]
 *
 ******************************************************************************/

void sleep_unblock_mode(uint32_t EM){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	EFM_ASSERT(lowest_energy_mode[EM] >= 0);

		lowest_energy_mode[EM]--;

	CORE_EXIT_CRITICAL();
}
/***************************************************************************//**
 * @brief (void)enter_sleep(void)
 * Enters Energy mode from EM0-EM3
 * @details
 * Checks lowest energy mode array and sets the energy mode.
 * @note
 *
 * @param[in] void function - no input
 *
 * @param[in]
 *
 ******************************************************************************/

void enter_sleep(void){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	if(lowest_energy_mode[EM0]>0){
		CORE_EXIT_CRITICAL();
		return;
	}
	else if(lowest_energy_mode[EM1]>0){
		CORE_EXIT_CRITICAL();
		return;
	}
	else if(lowest_energy_mode[EM2]>0){
		EMU_EnterEM1();
		CORE_EXIT_CRITICAL();
		return;
	}
	else if(lowest_energy_mode[EM3]>0){
		EMU_EnterEM2(true);
		CORE_EXIT_CRITICAL();
		return;
	}
	else{
		EMU_EnterEM3(true);
		CORE_EXIT_CRITICAL();
		return;
	}
}
/***************************************************************************//**
 * @brief
 *   Returns current energy mode of block
 *
 * @details
 *	Checks if the lowest energy mode is zero and returns the energy mode
 *
 *
 ******************************************************************************/

uint32_t current_block_energy_mode(void){
	int i;
	for(i=0;i<MAX_EM;i++){
		if (lowest_energy_mode[i] != 0)
			return i;
	}
	return MAX_EM -1;

}
/**************************************************************************
* @file sleep_routines.c
***************************************************************************
* @section License
* <b>(C) Copyright 2015 Silicon Labs, http://www.silabs.com</b>
***************************************************************************
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
* obligation to support this Software. Silicon Labs is providing the
* Software "AS IS", with no express or implied warranties of any kind,
* including, but not limited to, any implied warranties of merchantability
* or fitness for any particular purpose or warranties against infringement
* of any proprietary rights of a third party.
*
* Silicon Labs will not be liable for any consequential, incidental, or
* special damages, or any other relief, or for any claim by any third party,
* arising from your use of this Software.
*
**************************************************************************/
