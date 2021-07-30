/**
 * @file scheduler.c
 * @author Kay Sho
 * @date  10/4/2020
 * @brief Contains the driver function for the interrupt scheduler
 * @note The author's pronouns: (She/They)
 */

//***********************************************************************************
// Include files
//***********************************************************************************

/* System include statements */


/* Silicon Labs include statements */


/* The developer's include statements */
#include "scheduler.h"



//***********************************************************************************
// defined files
//***********************************************************************************

//***********************************************************************************
// private variables
//***********************************************************************************
static unsigned int event_scheduled;

#define SCHEDULE_CLR 		0; // Fixing magic number
//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *   resets static event variable
 *
 * @details
 *	Sets up event scheduler for usage by resetting to 0
 *
 * @note
 *
 * @param[in]
 *
 * @param[in]
 *
 ******************************************************************************/
void scheduler_open(void){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	event_scheduled = SCHEDULE_CLR;
	CORE_EXIT_CRITICAL();
}
/***************************************************************************//**
 * @brief
 *   Adds scheduled event.
 *
 * @details
 *	Alters the static variable s.t. [event] is played and filled into the static.
 *
 * @note
 *
 * @param[in] event
 *
 * Parameter is the desired event.
 *
 * @param[in]
 *
 ******************************************************************************/

void add_scheduled_event(uint32_t event){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	event_scheduled |= event;
	CORE_EXIT_CRITICAL();
}
/***************************************************************************//**
 * @brief
 *   Removes scheduled event.
 *
 * @details
 *	Alters the static variable s.t. [event] is removed once it is finished.
 *
 * @note
 *
 * @param[in] event
 *
 * Parameter is the desired event.
 *
 * @param[in]
 *
 ******************************************************************************/

void remove_scheduled_event(uint32_t event){
	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();
	event_scheduled &= ~event;
	CORE_EXIT_CRITICAL();
}
/***************************************************************************//**
 * @brief
 *   Returns scheduled event.
 *
 * @details
 * Does what it says.
 * @note
 *
 * @param[in]
 *
 * @param[in]
 *
 ******************************************************************************/

uint32_t get_scheduled_events(void){
	return event_scheduled;
}
