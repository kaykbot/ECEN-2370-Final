/**
 * @file leuart.c
 * @author Kay Sho
 * @date 11/1/20
 * @brief Contains all the functions of the LEUART peripheral
 *
 */

//***********************************************************************************
// Include files
//***********************************************************************************

//** Developer/user include files
#include "leuart.h"


//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// private variables
//***********************************************************************************
uint32_t	rx_done_evt;
uint32_t	tx_done_evt;

typedef enum {
		START,
		TRANSMIT,
		IDLE,
		CLOSE
}LEUART_TX_STATE;

typedef enum {
	STARTFRAME,
	RECEIVE,
	SIGFRAME
}LEUART_RX_STATE;

typedef struct{
	LEUART_TypeDef* 			leuart;				// LEUART Peripheral
	char						tx_out[50];				// Pointer to string data
	char						TXbuf[50];
	char						RXbuf[50];
	volatile bool				tx_busy;
	volatile bool				rx_busy;
	uint8_t						tx_len;
	uint8_t						rx_len;
	uint8_t						char_index;				// count
	LEUART_TX_STATE				tx_state;				// State
	LEUART_RX_STATE				rx_state;
}LEUART_COMMS_STRUCT;

static LEUART_COMMS_STRUCT leuart_sm;
/***************************************************************************//**
 * @brief LEUART driver
 * @details
 *  This module contains all the functions to support the driver's state
 *  machine to transmit a string of data across the LEUART bus.  There are
 *  additional functions to support the Test Driven Development test that
 *  is used to validate the basic set up of the LEUART peripheral.  The
 *  TDD test for this class assumes that the LEUART is connected to the HM-18
 *  BLE module.  These TDD support functions could be used for any TDD test
 *  to validate the correct setup of the LEUART.
 *
 ******************************************************************************/

//***********************************************************************************
// Private function prototypes
//***********************************************************************************

static void leuart_txbl(void);
static void leuart_txc(void);

static void leuart_startf(void);
static void leuart_rxdata(void);
static void leuart_sigf(void);


//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *		Handles the initialization of the LEUART
 * @details
 *		Function that sets up the LEUART peripheral for usage
 * @param[in]  *leuart
 * 		Defined LEUART struct
 * 	@param[in] *leuart_settings
 * 		Our defined leuart struct found in the .h file.
 *
 ******************************************************************************/

void leuart_open(LEUART_TypeDef *leuart, LEUART_OPEN_STRUCT *leuart_settings){
	LEUART_Init_TypeDef leuart_init;
	// 1) Enable LEUART peripheral clock
	if (leuart == LEUART0) { // Establish which LEUARTn peripheral we're using
		CMU_ClockEnable(cmuClock_LEUART0, true);
		} else {
			EFM_ASSERT(false);
		}
		// We should confirm clock enable is successful`
		leuart->STARTFRAME = 0x01;
		while(leuart->SYNCBUSY);
		EFM_ASSERT(leuart->STARTFRAME == 0x01);

		leuart->STARTFRAME = 0x00;
		while(leuart->SYNCBUSY);
		EFM_ASSERT(leuart->STARTFRAME == 0x00);
		// Set up
		leuart_init.baudrate = leuart_settings->baudrate;
		leuart_init.databits = leuart_settings->databits;
		leuart_init.enable	= 0;
		leuart_init.parity = leuart_settings->parity;
		leuart_init.refFreq = leuart_settings->refFreq;
		leuart_init.stopbits = leuart_settings->stopbits;

		LEUART_Reset(leuart);
		LEUART_Init(leuart, &leuart_init);
		while(leuart->SYNCBUSY);

		// 2) Route Peripheral Pins
		leuart->ROUTELOC0 = leuart_settings->rx_loc;
		leuart->ROUTELOC0 |= leuart_settings->tx_loc;

		if(leuart_settings ->rx_pin_en)
		{leuart->ROUTEPEN |= LEUART_ROUTEPEN_RXPEN;}
		if(leuart_settings ->tx_pin_en)
		{leuart->ROUTEPEN |= LEUART_ROUTEPEN_TXPEN;}

		// Set up Buffers and Interrupts
		leuart->CMD = LEUART_CMD_CLEARRX;
		leuart->CMD = LEUART_CMD_CLEARTX;
		while(leuart->SYNCBUSY)

		leuart->IFC = LEUART_IEN_TXC | LEUART_IEN_TXBL;

		LEUART_Enable(leuart, leuart_settings->enable);

		if(leuart_settings->rx_en){
			while(!(leuart->STATUS & LEUART_STATUS_RXENS));
			EFM_ASSERT(leuart->STATUS & LEUART_STATUS_RXENS);
		}
		if(leuart_settings->tx_en){
			while(!(leuart->STATUS & LEUART_STATUS_TXENS));
			EFM_ASSERT(leuart->STATUS & LEUART_STATUS_TXENS);
		}
		while(leuart->SYNCBUSY);

		if(leuart == LEUART0){
			NVIC_EnableIRQ(LEUART0_IRQn);
		} else {EFM_ASSERT(false);}

		rx_done_evt = leuart_settings->rx_done_evt;
		tx_done_evt = leuart_settings->tx_done_evt;

		// Lab 7 LEUART development additions below:
			leuart->CTRL |= LEUART_CTRL_SFUBRX;
				while(leuart->SYNCBUSY);
				// Among list of LEUART registers that requires a stall to use properly.
			leuart->CMD = LEUART_CMD_RXBLOCKEN;
				// the CMD register is a write-only register, so making it |= has no effect on it.
				while(leuart->SYNCBUSY);
			leuart->STARTFRAME = '#';
				while(leuart->SYNCBUSY);
			leuart->SIGFRAME = '!';
				while(leuart->SYNCBUSY);

			leuart_sm.rx_busy = false;
			leuart_sm.rx_state = START;

			sleep_block_mode(LEUART_EM);

		// Simple line of code to clear interrupts
		leuart->IFC = leuart->IF; // This line already clears RXDATAV and SIGF Interrupts

		// Initialize the interrupts
		leuart->IEN |= LEUART_IEN_STARTF;
		leuart->IEN &= ~LEUART_IEN_RXDATAV;
		leuart->IEN &= ~LEUART_IEN_SIGF;
		leuart_sm.tx_state = IDLE;

		leuart_loopbk_test(LEUART0);

}

/***************************************************************************//**
 * @brief
 * 		IRQ Handler for LEUART0
 * @details
 * 		Only Interrupts are TXBL and TXC
 * @note
 * 		For more information, see the leuart_txbl()
 * 		and leuart_txc()
 ******************************************************************************/

void LEUART0_IRQHandler(void){
	uint32_t int_flag = LEUART0->IF & LEUART0->IEN;
	LEUART0->IFC = int_flag;
	if(int_flag & LEUART_IF_TXBL){
		leuart_txbl();
	}
	if(int_flag & LEUART_IF_TXC){
		leuart_txc();
	}
	if(int_flag & LEUART_IF_STARTF){
		leuart_startf();
	}
	if(int_flag & LEUART_IF_RXDATAV){
		leuart_rxdata();
	}
	if(int_flag & LEUART_IF_SIGF){
		leuart_sigf();
	}
}

/***************************************************************************//**
 * @brief
 * 		Initializes the LEUART function for use.
 * @details
 *		The initialization function for the LEUART once the LEUART is configured
 * @param[in]  *leuart
 * 		Defined leuart struct
 * @param[in]  *string
 * 		Input string
 * @param[in]  string_len
 * 		Length of the indicated input string being pointed to
 * @note
 * 		The atomic operations MUST BE below the while loop above. This is because
 * 		we will be preventing IRQ that is currently running from completing.
 *
 * 		We want this code sandwiched between the CORE_ENTER_CRITICAL() and the CORE_EXIT_CRITICAL
 * 		so that in -O2 optimization, that sandwiched code will run linearly.
 ******************************************************************************/

void leuart_start(LEUART_TypeDef *leuart, char *string, uint32_t string_len){
	while(leuart_tx_busy());

	EFM_ASSERT(leuart->STATUS & LEUART_STATUS_TXIDLE);
	EFM_ASSERT(string_len > 0); // Check if the string has text

	CORE_DECLARE_IRQ_STATE; // Checking to see if global interrupts are enabled
	CORE_ENTER_CRITICAL(); // Make the operation atomic

	leuart_sm.tx_len = string_len;
	leuart_sm.char_index = 0;
	leuart_sm.leuart = leuart;
	strcpy(leuart_sm.tx_out, string);
	leuart_sm.tx_busy = true;
	sleep_block_mode(LEUART_EM);
	leuart_sm.tx_state = TRANSMIT;
	leuart->IEN |= LEUART_IEN_TXBL;

	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 * 		Handles the case when tx is busy.
 * @details
 *		Returns the state of the leuart_tx_busy bit.
 ******************************************************************************/
bool leuart_tx_busy(void){
		return leuart_sm.tx_busy;
}
/***************************************************************************//**
 * @brief
 * 		Handles the case when rx is busy.
 * @details
 *		Checks the state of the rx_busy bit of the LEUART
 ******************************************************************************/
bool leuart_rx_busy(void){
		return leuart_sm.rx_busy;
}
/***************************************************************************//**
 * @brief
 *		Handles the TXBL for LEUART
 * @details
 *		Switch statement for State Machine when the TXBL Interrupt is handled
 *		for data transmission.
 * @note
 * 		Please note that we don't actually use two of the defined states.
 * 		Could we possibly rid of them for simplicity?
 ******************************************************************************/
void leuart_txbl(void){
	switch(leuart_sm.tx_state){
		case IDLE:
			EFM_ASSERT(false);
			break;
		case START:
			EFM_ASSERT(false);
			break;
		case TRANSMIT:
			leuart_sm.leuart->TXDATA = leuart_sm.tx_out[leuart_sm.char_index];
			leuart_sm.char_index++;
			if(leuart_sm.char_index >= leuart_sm.tx_len){
				leuart_sm.leuart->IEN &= ~LEUART_IEN_TXBL;
				leuart_sm.leuart->IEN |= LEUART_IEN_TXC;
				leuart_sm.tx_state = CLOSE;
			}
			break;
		case CLOSE:
			EFM_ASSERT(false);
			break;
	}
}
/***************************************************************************//**
 * @brief
 *		Handles the TXC for leuart.
 * @details
 *		Switch statement for State Machine when the TXC interrupt is run
 * @note
 * 		Because we have finished transmitting, we must set the tx_busy bit to false
 ******************************************************************************/
void leuart_txc(void){
	switch(leuart_sm.tx_state){
		case IDLE:
			EFM_ASSERT(false);
			break;
		case START:
			EFM_ASSERT(false);
			break;
		case TRANSMIT:
			EFM_ASSERT(false);
			break;
		case CLOSE:

			leuart_sm.tx_busy = false;
			leuart_sm.leuart->IEN &= ~LEUART_IEN_TXC;
			leuart_sm.tx_state = IDLE;
			add_scheduled_event(tx_done_evt);
			sleep_unblock_mode(LEUART_EM);
			break;
	}
}

/***************************************************************************//**
 * @brief
 *		Handles the STARTF interrupt for leuart.
 * @details
 *		Switch statement RX State machine that handles the case for when the
 *		Start Frame is detected
 * @note
 * 		It is important to set the RX busy bit to true so that
 * 		the interrupt may proceed.
 ******************************************************************************/
void leuart_startf(void){
	switch(leuart_sm.rx_state){
		case STARTFRAME:
					leuart_sm.rx_busy = true;
					leuart_sm.rx_len = 0;
					LEUART0->IFC = LEUART_IEN_RXDATAV;
					LEUART0->IFC = LEUART_IEN_SIGF;
					LEUART0->IEN |= LEUART_IEN_RXDATAV;
					LEUART0->IEN |= LEUART_IEN_SIGF;
					leuart_sm.rx_state = RECEIVE;
			break;
		case RECEIVE:
			EFM_ASSERT(false);
			break;
		case SIGFRAME:
			EFM_ASSERT(false);
			break;
	}
}

/***************************************************************************//**
 * @brief
 *		Handles the RXDATA interrupt for leuart.
 * @details
 *		Switch statement RX State machine that handles the case for when the
 *		Start Frame is detected
 ******************************************************************************/
void leuart_rxdata(void){
	switch(leuart_sm.rx_state){
		case STARTFRAME:
			EFM_ASSERT(false);
			break;
		case RECEIVE:
			leuart_sm.RXbuf[leuart_sm.rx_len] = leuart_sm.leuart->RXDATA;
			leuart_sm.rx_len++;
			break;
		case SIGFRAME:
			EFM_ASSERT(false);
			break;
	}
}

/***************************************************************************//**
 * @brief
 *		Handles the STARTF interrupt for leuart.
 * @details
 *		Switch statement RX State machine that handles the case for when the
 *		Start Frame is detected
 * @note
 * 		We must set the leuart0_rx_busy to false so that the RX action can be done
 ******************************************************************************/
void leuart_sigf(void){
	switch(leuart_sm.rx_state){
		case STARTFRAME:
			EFM_ASSERT(false);
			break;
		case RECEIVE:
			leuart_sm.rx_busy = false;
			add_scheduled_event(rx_done_evt);
			leuart_sm.rx_state = STARTFRAME;
			leuart_sm.leuart->CMD = LEUART_CMD_RXBLOCKEN;
			while(LEUART0->SYNCBUSY);
			leuart_sm.leuart->IEN |= LEUART_IEN_STARTF;
			leuart_sm.RXbuf[leuart_sm.rx_len] = 0;
			leuart_sm.leuart->IEN &= ~LEUART_IEN_SIGF;
			leuart_sm.leuart->IEN &= ~LEUART_IEN_RXDATAV;
			break;
		case SIGFRAME:
			EFM_ASSERT(false);
			break;
	}
}

/***************************************************************************//**
 * @brief
 *   LEUART STATUS function returns the STATUS of the peripheral for the
 *   TDD test
 *
 * @details
 * 	 This function enables the LEUART STATUS register to be provided to
 * 	 a function outside this .c module.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the STATUS register value as an uint32_t value
 *
 ******************************************************************************/

uint32_t leuart_status(LEUART_TypeDef *leuart){
	uint32_t	status_reg;
	status_reg = leuart->STATUS;
	return status_reg;
}

/***************************************************************************//**
 * @brief
 *   LEUART CMD Write sends a command to the CMD register
 *
 * @details
 * 	 This function is used by the TDD test function to program the LEUART
 * 	 for the TDD tests.
 *
 * @note
 *   Before exiting this function to update  the CMD register, it must
 *   perform a SYNCBUSY while loop to ensure that the CMD has by synchronized
 *   to the lower frequency LEUART domain.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] cmd_update
 * 	 The value to write into the CMD register
 *
 ******************************************************************************/

void leuart_cmd_write(LEUART_TypeDef *leuart, uint32_t cmd_update){

	leuart->CMD = cmd_update;
	while(leuart->SYNCBUSY);
}

/***************************************************************************//**
 * @brief leuart_if_reset
 *   LEUART IF Reset resets all interrupt flag bits that can be cleared
 *   through the Interrupt Flag Clear register
 *
 * @details
 * 	 This function is used by the TDD test program to clear interrupts before
 * 	 the TDD tests and to reset the LEUART interrupts before the TDD
 * 	 exits
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 ******************************************************************************/

void leuart_if_reset(LEUART_TypeDef *leuart){
	leuart->IFC = 0xffffffff;
}

/***************************************************************************//**
 * @brief
 *   LEUART App Transmit Byte transmits a byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a transmit byte, a while statement checking for the TXBL
 *   bit in the Interrupt Flag register is required before writing the
 *   TXDATA register.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @param[in] data_out
 *   Byte to be transmitted by the LEUART peripheral
 *
 ******************************************************************************/

void leuart_app_transmit_byte(LEUART_TypeDef *leuart, uint8_t data_out){
	while (!(leuart->IF & LEUART_IF_TXBL));
	leuart->TXDATA = data_out;
}


/***************************************************************************//**
 * @brief
 *   LEUART App Receive Byte polls a receive byte for the LEUART TDD test
 *
 * @details
 * 	 The BLE module will respond to AT commands if the BLE module is not
 * 	 connected to the phone app.  To validate the minimal functionality
 * 	 of the LEUART peripheral, write and reads to the LEUART will be
 * 	 performed by polling and not interrupts.
 *
 * @note
 *   In polling a receive byte, a while statement checking for the RXDATAV
 *   bit in the Interrupt Flag register is required before reading the
 *   RXDATA register.
 *
 * @param[in] *leuart
 *   Defines the LEUART peripheral to access.
 *
 * @return
 * 	 Returns the byte read from the LEUART peripheral
 *
 ******************************************************************************/

uint8_t leuart_app_receive_byte(LEUART_TypeDef *leuart){
	uint8_t leuart_data;
	while (!(leuart->IF & LEUART_IF_RXDATAV));
	leuart_data = leuart->RXDATA;
	return leuart_data;
}

/***************************************************************************//**
 * @brief
 *   	Exports the received data.
 * @details
 * 		Exports the RXdata saved in the RX buffer for external use.
 * @param [in] *string
 *		This string is the data retrieved from the RXData buffer to be exported.
 ******************************************************************************/
void received_data(char*string){
	strcpy(string,leuart_sm.RXbuf);
}
/****************************************************************************//**
 * @brief
 *		LEUART TX/RX functionality test
 * @details
 *		Test the LEUART's TX/RX line by using Loopback to talk to itself. This test
 *		checks the interrupts and acts as prototyping for the project.
 * @param[in] *leuart
 * 		Basic LEUART struct.
 * @note
 *		LEUn_TX pin must be enabled as an output in the GPIO
 *		LEUART0_CTRL -> LOOPBK enable -> disable
 * @note
 * 		It is important to clear the buffers before exiting the TDD so that the
 * 		transmitted strings aren't treated as a command being fed into the RX

 ******************************************************************************/
void leuart_loopbk_test(LEUART_TypeDef *leuart){
	uint32_t int_flag;
	int_flag = leuart->IEN;
	leuart->IEN = 0; // Clear Interrupt Enable

	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	leuart->CTRL |= LEUART_CTRL_LOOPBK;
	while(leuart->SYNCBUSY);

	// The TDD for the LEUART is to simulate the TX for the LEUART peripheral.
	// Specifically, we will be sending data from the TX line and have it use
	// the Loopback feature found on the LEUART to have it be received by the
	// RX line. The test will simulate us sending a command to/from the
	// connected bluetooth device. Essentially, we are having the PG12 talk to
	// itself.

	// In order to have this done, we must enable the Loopback bit, set by the
	// CTRL register, as well as set the LEUn_TX in the GPIO mode to Output.

	// Our tests will follow the same structure each time.
	// Test one checks if the StartFrame detection works, and thus, if the RXDATA
	// is enabled. (RXBLOCK)

	EFM_ASSERT(leuart->STATUS & LEUART_STATUS_RXBLOCK);

	// Test one checks sending garbage before sending Start Frame.
	leuart->TXDATA = '1';
	timer_delay(4); 		// We include this to make sure the transmission went.
	EFM_ASSERT(!(leuart->IF & LEUART_IF_STARTF));
	EFM_ASSERT(!(LEUART_IF_RXDATAV & leuart->IF));
	leuart->IFC |= leuart->IF; // Clear all flags

	// Test two checks the StartFrame incrementally.
	leuart->TXDATA = '#';
	timer_delay(4);
	EFM_ASSERT(leuart->RXDATA == '#');
	EFM_ASSERT(leuart->IF & LEUART_IF_STARTF); // Check if the StartFrame interrupt flag was set.
	EFM_ASSERT(!(leuart->STATUS & LEUART_STATUS_RXBLOCK)); // Check if RX is still blocked
	leuart->IFC = leuart->IF;

	// Test three sends a string, with the StartFrame still active from test two.
	leuart->TXDATA = 'A';
	timer_delay(4);
	EFM_ASSERT(LEUART_IF_RXDATAV & leuart->IF); // Check if data is being received in the RX.
	EFM_ASSERT(leuart->RXDATA == 'A');
	EFM_ASSERT(!(leuart->STATUS & LEUART_STATUS_RXBLOCK));
	EFM_ASSERT(!(LEUART_IF_STARTF & leuart->IF));
	leuart->IFC = leuart->IF;

	// Test four sends the defined SigFrame to finish the transmission process.
	leuart->TXDATA = '!';
	timer_delay(4);
	EFM_ASSERT(leuart->RXDATA == '!');
	EFM_ASSERT(leuart->IF & LEUART_IF_SIGF); // Same as test two, but checking SIGF, not STARTF
	EFM_ASSERT(!(leuart->STATUS & LEUART_STATUS_RXBLOCK));
	leuart->IFC = leuart->IF;

	CORE_EXIT_CRITICAL();
	leuart->IFC = leuart->IF; // It is best practice to first clear interrupts
	leuart->IEN = int_flag; //Turns on interrupts again

	leuart->CMD = LEUART_CMD_RXBLOCKEN;
	while(leuart->SYNCBUSY);

	//Test five
	char tx5[50] = "Bad#Good!Bad";
	char good_str[50] = "#Good!";
	char res_str[50];
	uint8_t str_len = strlen(tx5);
	leuart_start(leuart, tx5, str_len);
	while(leuart_tx_busy());
	while(leuart_rx_busy());
	strcpy(res_str, leuart_sm.RXbuf); // copying buffer into result
	for(int i =0; i < strlen(good_str); i++){
		EFM_ASSERT(res_str[i] == good_str[i]);
	}

	remove_scheduled_event(tx_done_evt);// We now must clear the buffer via clearing the event
	remove_scheduled_event(rx_done_evt);// These two lines of code are for the express purpose of clearing the buffer
	EFM_ASSERT((leuart->STATUS & LEUART_STATUS_RXBLOCK)); // Check if RX is blocked

	leuart->CTRL &= ~LEUART_CTRL_LOOPBK;
	while(leuart->SYNCBUSY);
}
