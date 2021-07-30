//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	LEUART_GUARD_H
#define	LEUART_GUARD_H

//** Standard Library includes
#include <string.h>

//** Silicon Labs include files
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_leuart.h"

#include "sleep_routines.h"
#include "scheduler.h"
#include "HW_delay.h"



//***********************************************************************************
// defined files
//***********************************************************************************

#define LEUART_EM		EM3

#define STARTF_CHAR		(uint8_t) '#'
#define SIGF_CHAR		(uint8_t) '!'
/***************************************************************************//**
 * @addtogroup leuart
 * @{
 ******************************************************************************/

typedef struct {
	uint32_t					baudrate;
	LEUART_Databits_TypeDef		databits;
	LEUART_Enable_TypeDef		enable;
	LEUART_Parity_TypeDef 		parity;
	LEUART_Stopbits_TypeDef		stopbits;
	uint32_t					refFreq;

	bool						rxblocken;
	bool						sfubrx;
	bool						startframe_en;
	char						startframe;
	bool						sigframe_en;
	char						sigframe;

	uint32_t					rx_loc;
	uint32_t					rx_pin_en;
	uint32_t					tx_loc;
	uint32_t					tx_pin_en;
	bool						rx_en;
	bool						tx_en;
	uint32_t					rx_done_evt;
	uint32_t					tx_done_evt;
} LEUART_OPEN_STRUCT;

/** @} (end addtogroup leuart) */

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void leuart_open(LEUART_TypeDef *leuart, LEUART_OPEN_STRUCT *leuart_settings);
void LEUART0_IRQHandler(void);
void leuart_start(LEUART_TypeDef *leuart, char *string, uint32_t string_len);
bool leuart_tx_busy(void);
bool leuart_rx_busy(void);

uint32_t leuart_status(LEUART_TypeDef *leuart);
void leuart_cmd_write(LEUART_TypeDef *leuart, uint32_t cmd_update);
void leuart_if_reset(LEUART_TypeDef *leuart);
void leuart_app_transmit_byte(LEUART_TypeDef *leuart, uint8_t data_out);
uint8_t leuart_app_receive_byte(LEUART_TypeDef *leuart);

void leuart_loopbk_test(LEUART_TypeDef *leuart);
void received_data(char*string);

#endif
