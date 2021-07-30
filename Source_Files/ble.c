/**
 * @file ble.c
 * @author
 * @date
 * @brief Contains all the functions to interface the application with the HM-18
 *   BLE module and the LEUART driver
 *
 */


//***********************************************************************************
// Include files
//***********************************************************************************
#include "ble.h"


//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// private variables
//***********************************************************************************
CIRC_TEST_STRUCT test_struct;
static BLE_CIRCULAR_BUF ble_cbuf;
static char pop_str[CSIZE];
/***************************************************************************//**
 * @brief BLE module
 * @details
 *  This module contains all the functions to interface the application layer
 *  with the HM-18 Bluetooth module.  The application does not have the
 *  responsibility of knowing the physical resources required, how to
 *  configure, or interface to the Bluetooth resource including the LEUART
 *  driver that communicates with the HM-18 BLE module.
 *
 ******************************************************************************/

//***********************************************************************************
// Private functions
//***********************************************************************************
static void ble_circ_init(void);
static void ble_circ_push(char *string);
static uint8_t ble_circ_space(void);
static void update_circ_wrtindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by);
static void update_circ_readindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by);
//***********************************************************************************
// Global functions
//***********************************************************************************


/***************************************************************************//**
 * @brief
 * 		Handles the initialization of the leuart for bluetooth LE
 * @details
 * 		Initializes the ble device with necessary data for the device to
 * 		run properly
 * @param[in] tx_event
 * 		Event to indicate transmission of data
 * @param[in] rx_event
 * 		Event to indicate reception of data
 ******************************************************************************/

void ble_open(uint32_t tx_event, uint32_t rx_event){

	LEUART_OPEN_STRUCT ble_leuart;

	ble_circ_init();

	// Bluetooth initialization
	ble_leuart.baudrate = HM10_BAUDRATE;
	ble_leuart.databits = HM10_DATABITS;
	ble_leuart.enable = HM10_ENABLE;
	ble_leuart.refFreq = HM10_REFFREQ;
	ble_leuart.stopbits = HM10_STOPBITS;
	ble_leuart.parity = HM10_PARITY;

	// RX bit initialization
	ble_leuart.rx_done_evt = rx_event;
	ble_leuart.rx_loc = LEUART0_RX_ROUTE;
	ble_leuart.rx_en = LEUART0_RX_ENABLE;
	ble_leuart.rx_pin_en = LEUART0_RX_ENABLE;

	// TX bit initialization
	ble_leuart.tx_done_evt = tx_event;
	ble_leuart.tx_loc = LEUART0_TX_ROUTE;
	ble_leuart.tx_en = LEUART0_TX_ENABLE;
	ble_leuart.tx_pin_en = LEUART0_TX_ENABLE;

	leuart_open(HM10_LEUART0, &ble_leuart);
}


/***************************************************************************//**
 * @brief
 *  	Function that performs the action of writing string to the device
 * @details
 * 		Only parses the one line where the string is passed into the LEUART
 * 		and written for it to be sent from th ble device
 * @note
 *
 ******************************************************************************/

void ble_write(char* string){
	ble_circ_push(string);
	ble_circ_pop(false);
//	leuart_start(HM10_LEUART0, string, strlen(string));
}

/***************************************************************************//**
 * @brief
 *   BLE Test performs two functions.  First, it is a Test Driven Development
 *   routine to verify that the LEUART is correctly configured to communicate
 *   with the BLE HM-18 module.  Second, the input argument passed to this
 *   function will be written into the BLE module and become the new name
 *   advertised by the module while it is looking to pair.
 *
 * @details
 * 	 This global function will use polling functions provided by the LEUART
 * 	 driver for both transmit and receive to validate communications with
 * 	 the HM-18 BLE module.  For the assignment, the communication with the
 * 	 BLE module must use low energy design principles of being an interrupt
 * 	 driven state machine.
 *
 * @note
 *   For this test to run to completion, the phone most not be paired with
 *   the BLE module.  In addition for the name to be stored into the module
 *   a breakpoint must be placed at the end of the test routine and stopped
 *   at this breakpoint while in the debugger for a minimum of 5 seconds.
 *
 * @param[in] *mod_name
 *   The name that will be written to the HM-18 BLE module to identify it
 *   while it is advertising over Bluetooth Low Energy.
 *
 * @return
 *   Returns bool true if successfully passed through the tests in this
 *   function.
 ******************************************************************************/

bool ble_test(char *mod_name){
	uint32_t	str_len;

	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	// This test will limit the test to the proper setup of the LEUART
	// peripheral, routing of the signals to the proper pins, pin
	// configuration, and transmit/reception verification.  The test
	// will communicate with the BLE module using polling routines
	// instead of interrupts.
	// How is polling different than using interrupts?

	// ANSWER: Polling actively 'polls' the CPU Cycles for commands, whereas interrupts
	// allow the CPU to enter sleep mode.

	// How does interrupts benefit the system for low energy operation?

	// ANSWER: Since the system is asleep until an interrupt happens, the overall system
	// remains in low energy consumption.

	// How does interrupts benefit the system that has multiple tasks?

	// ANSWER: Through interrupts, multiple tasks can be initiated at a time and awaits a
	// response, rather than having to wait for a task to fully complete before moving on
	// to a new task.

	// First, you will need to review the DSD HM10 datasheet to determine
	// what the default strings to write data to the BLE module and the
	// expected return statement from the BLE module to test / verify the
	// correct response

	// The test_str is used to tell the BLE module to end a Bluetooth connection
	// such as with your phone.  The ok_str is the result sent from the BLE module
	// to the micro-controller if there was not active BLE connection at the time
	// the break command was sent to the BLE module.
	// Replace the test_str "" with the command to break or end a BLE connection
	// Replace the ok_str "" with the result that will be returned from the BLE
	//   module if there was no BLE connection
	char		test_str[80] = "AT";
	char		ok_str[80] = "OK";


	// output_str will be the string that will program a name to the BLE module.
	// From the DSD HM10 datasheet, what is the command to program a name into
	// the BLE module?
	// The  output_str will be a string concatenation of the DSD HM10 command
	// and the input argument sent to the ble_test() function
	// Replace the output_str "" with the command to change the program name
	// Replace the result_str "" with the first part of the expected result
	//  the backend of the expected response will be concatenated with the
	//  input argument
	char		output_str[80] = "AT+NAME";
	char		result_str[80] = "OK+Set:";


	// To program the name into your module, you must reset the module after you
	// have sent the command to update the modules name.  What is the DSD HM10
	// name to reset the module?
	// Replace the reset_str "" with the command to reset the module
	// Replace the reset_result_str "" with the expected BLE module response to
	//  to the reset command
	char		reset_str[80] = "AT+RESET";
	char		reset_result_str[80] = "OK+RESET";
	char		return_str[80];

	bool		success;
	bool		rx_disabled, rx_en, tx_en;
	uint32_t	status;

	// These are the routines that will build up the entire command and response
	// of programming the name into the BLE module.  Concatenating the command or
	// response with the input argument name
	strcat(output_str, mod_name);
	strcat(result_str, mod_name);

	// The test routine must not alter the function of the configuration of the
	// LEUART driver, but requires certain functionality to insure the logical test
	// of writing and reading to the DSD HM10 module.  The following c-lines of code
	// save the current state of the LEUART driver that will be used later to
	// re-instate the LEUART configuration

	status = leuart_status(HM10_LEUART0);
	if (status & LEUART_STATUS_RXBLOCK) {
		rx_disabled = true;
		// Enabling, unblocking, the receiving of data from the LEUART RX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXBLOCKDIS);
	}
	else rx_disabled = false;
	if (status & LEUART_STATUS_RXENS) {
		rx_en = true;
	} else {
		rx_en = false;
		// Enabling the receiving of data from the RX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXEN);
		while (!(leuart_status(HM10_LEUART0) & LEUART_STATUS_RXENS));
	}

	if (status & LEUART_STATUS_TXENS){
		tx_en = true;
	} else {
		// Enabling the transmission of data to the TX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_TXEN);
		while (!(leuart_status(HM10_LEUART0) & LEUART_STATUS_TXENS));
		tx_en = false;
	}
//	leuart_cmd_write(HM10_LEUART0, (LEUART_CMD_CLEARRX | LEUART_CMD_CLEARTX));

	// This sequence of instructions is sending the break ble connection
	// to the DSD HM10 module.
	// Why is this command required if you want to change the name of the
	// DSD HM10 module?
	// ANSWER:
	str_len = strlen(test_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, test_str[i]);
	}

	// What will the ble module response back to this command if there is
	// a current ble connection?
	// ANSWER:
	str_len = strlen(ok_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (ok_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// This sequence of code will be writing or programming the name of
	// the module to the DSD HM10
	str_len = strlen(output_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, output_str[i]);
	}

	// Here will be the check on the response back from the DSD HM10 on the
	// programming of its name
	str_len = strlen(result_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (result_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// It is now time to send the command to RESET the DSD HM10 module
	str_len = strlen(reset_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, reset_str[i]);
	}

	// After sending the command to RESET, the DSD HM10 will send a response
	// back to the micro-controller
	str_len = strlen(reset_result_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (reset_result_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// After the test and programming have been completed, the original
	// state of the LEUART must be restored
	if (!rx_en) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXDIS);
	if (rx_disabled) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXBLOCKEN);
	if (!tx_en) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_TXDIS);
	leuart_if_reset(HM10_LEUART0);

	success = true;


	CORE_EXIT_CRITICAL();
	return success;
}

/***************************************************************************//**
 * @brief
 * 		Handles starting the BLE circular buffer.
 * @details
 *		Initializes the Bluetooth Low-Energy circular buffer.
 *		We set everything to 0 to ensure no leftovers are used or overwritten.
 ******************************************************************************/
void ble_circ_init(void){
	memset(&ble_cbuf.cbuf[0], 0, CSIZE);
	ble_cbuf.size_mask = CSIZE-CIRC_MN;
	ble_cbuf.size = 0;
	ble_cbuf.read_ptr = 0;
	ble_cbuf.write_ptr = 0;
}

/***************************************************************************//**
 * @brief
 * 		Handles the BLE circular Push Buffer
 *
 * @details
 *		Handles the Bluetooth Low-Energy circular push (write) buffer.
 *		We write or "push" a string onto the circular buffer.
 *
 * @note
 * 		We must perform atomic operations for this, and the pull or "pop"
 *
 * @param[in] *string
 * 		String that will be "pushed" into the circ. buffer.
 ******************************************************************************/
void ble_circ_push(char* string){
	CORE_DECLARE_IRQ_STATE; //Atomic Operations
	CORE_ENTER_CRITICAL();
	uint8_t str_len = strlen(string); //Enter string into cup
	// S0: Check
	// S1: Write
	// S2: Update
	if(str_len==0){ // Want to check if string is empty
		CORE_EXIT_CRITICAL();
		return; // This is part of S0
	}

	// As described in the Doxygen for ble_circ_space, we use it to address spaces
	uint8_t space = 0;
	space = ble_circ_space();
	EFM_ASSERT(space != 0); // Space check complete; part of S0

	uint8_t pac_len = str_len + CIRC_MN;
	EFM_ASSERT(pac_len <= space); // Check for overflow; part of S0

	ble_cbuf.cbuf[ble_cbuf.write_ptr] = pac_len; // push packet into buffer
	update_circ_wrtindex(&ble_cbuf, CIRC_MN); // update write ptr
	ble_cbuf.size++; // This is all part of S1 & S2

	int i; // circular
	for(i=0; i < str_len; i++){
		ble_cbuf.cbuf[ble_cbuf.write_ptr] = string[i];
		update_circ_wrtindex(&ble_cbuf, CIRC_MN);
		ble_cbuf.size++;
	}
	CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 * 		Handles the BLE circular pop Buffer
 *
 * @details
 *		Pops off a packet from the circular buffer if the LEUART is busy
 *
 * @note
 * 		This function transmit string data over LEUART
 *
 * @param[in] test
 * 		boolean test
 ******************************************************************************/
bool ble_circ_pop(bool test){
	CORE_DECLARE_IRQ_STATE; //Atomic Operations
	CORE_ENTER_CRITICAL();
	// Must have leuart be in idle
	if(leuart_tx_busy()){ //fails test bool if LEUART not in IDLE state
		CORE_EXIT_CRITICAL();
		return false;
	}
	if(ble_cbuf.size == 0){
		CORE_EXIT_CRITICAL();
		return true;
	}

	// Block deals with pop packet
	EFM_ASSERT(ble_cbuf.size != CIRC_MN); // Malformed packet
	uint8_t str_len = ble_cbuf.cbuf[ble_cbuf.read_ptr] - CIRC_MN;
	update_circ_readindex(&ble_cbuf,CIRC_MN);
	ble_cbuf.size--;

	//Block deals with string
	EFM_ASSERT(ble_cbuf.size >= str_len);
	int i;
	for(i=0; i<str_len;i++){
		pop_str[i] = ble_cbuf.cbuf[ble_cbuf.read_ptr];
		update_circ_readindex(&ble_cbuf,CIRC_MN);
		ble_cbuf.size--;
	}
	pop_str[str_len] = 0;

	if(test){
		memcpy(test_struct.result_str, pop_str, str_len + CIRC_MN);
	} else {
		leuart_start(HM10_LEUART0, pop_str, strlen(pop_str));
	}
	CORE_EXIT_CRITICAL();
	return false;
}

/***************************************************************************//**
 * @brief
 * 		Handles spaces
 * @details
 *		Checks and handles number of spaces in string.
 * @return
 * 		8 bit integer of # of spaces detected in string found in buffer
 ******************************************************************************/
static uint8_t ble_circ_space(void){
	return CSIZE - ble_cbuf.size;
}

/***************************************************************************//**
 * @brief update_circ_wrtindex()
 * 		Updates circular buffer write index
 * @details
 *		Performs fast modulo via incrementing write pointer by the update_by param,
 *		then bitwise AND w/ size mask.
 * @note
 * 		As noted in class, prelab videos, and textbook, length of the buffer must
 * 		be power of 2.
 * @note
 * 		Notice how similar the write/push procedure is to the push/pop/read procedure is.
 * @param[in] *index_struct
 *		Circular buffer struct update pointer
 * @param[in] update_by
 *		Unsigned 32-bit integer value used to update index (write)
 ******************************************************************************/
static void update_circ_wrtindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by){
	index_struct->write_ptr = (index_struct->write_ptr + update_by) & index_struct->size_mask;
}

/***************************************************************************//**
 * @brief
 * 		Updates circular buffer read index
 * @details
 *		Performs fast modulo via incrementing read pointer by the update_by param,
 *		then bitwise AND w/ size mask. This is essentially the same process as the
 *		update_circ_wrtindex private function
 * @note
 * 		As noted in class, prelab videos, and textbook, length of the buffer must
 * 		be power of 2.
 * @param[in] *index_struct
 *		indicated index struct
 * @param[in]  update_by
 *		Unsigned 32-bit integer value used to update index (read_
 ******************************************************************************/
static void update_circ_readindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by){
	index_struct->read_ptr = (index_struct->read_ptr + update_by) & index_struct->size_mask;
}
/***************************************************************************//**
 * @brief
 *   Circular Buff Test is a Test Driven Development function to validate
 *   that the circular buffer implementation
 *
 * @details
 * 	 This Test Driven Development test has tests integrated into the function
 * 	 to validate that the routines can successfully identify whether there
 * 	 is space available in the circular buffer, the write and index pointers
 * 	 wrap around, and that one or more packets can be pushed and popped from
 * 	 the circular buffer.
 *
 * @note
 *   If anyone of these test will fail, an EFM_ASSERT will occur.  If the
 *   DEBUG_EFM=1 symbol is defined for this project, exiting this function
 *   confirms that the push, pop, and the associated utility functions are
 *   working.
 *
 * @par
 *   There is a test escape that is not possible to test through this
 *   function that will need to be verified by writing several ble_write()s
 *   back to back and verified by checking that these ble_write()s were
 *   successfully transmitted to the phone app.
 *
 ******************************************************************************/

 void circular_buff_test(void){
	 bool buff_empty;
	 int test1_len = 50;
	 int test2_len = 25;
	 int test3_len = 5;

	 // Why this 0 initialize of read and write pointer?
	 // Student Response:
	 // We start by pointing both buffers at the 0th index, since the buffer is empty
	 ble_cbuf.read_ptr = 0;
	 ble_cbuf.write_ptr = 0;

	 // Why do none of these test strings contain a 0?
	 // Student Response:
	 // Putting in a 0 into the test will result in a NULL, which will just result in
	 // random integers, and not the information we want to print out..
	 for (int i = 0;i < test1_len; i++){
		 test_struct.test_str[0][i] = i+1;
	 }
	 test_struct.test_str[0][test1_len] = 0;

	 for (int i = 0;i < test2_len; i++){
		 test_struct.test_str[1][i] = i + 20;
	 }
	 test_struct.test_str[1][test2_len] = 0;

	 for (int i = 0;i < test3_len; i++){
		 test_struct.test_str[2][i] = i +  35;
	 }
	 test_struct.test_str[2][test3_len] = 0;

	 // What is this test validating?
	 // Student response:
	 // This test is validating that the value of ble_circ_space is 64, the defined size of CSIZE.
	 // Note that CSIZE is defined as 64, and we are being fed the value of test3_len.
	 // This counts the number of spaces counted in the above test.
	 EFM_ASSERT(ble_circ_space() == CSIZE);

	 // Why is there only one push to the circular buffer at this stage of the test
	 // Student Response:
	 // This test is testing "good" instances of push, not extraneous(?) cases
	 ble_circ_push(&test_struct.test_str[0][0]);

	 // What is this test validating?
	 // Student response:
	 // Given test1_len, it tests the function of ble_circ_space so that
	 // the assigned value of ble_circ_space is 64-50-1 = 13.
	 EFM_ASSERT(ble_circ_space() == (CSIZE - test1_len - 1));

	 // Why is the expected buff_empty test = false?
	 // Student Response:
	 // When ble_circ_pop is run, it will return false so as long as the buffer is empty.
	 // IF not, then the assertion fails, since if the buffer is empty, then there's nothing
	 // to 'pop' or read.
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test1_len; i++){
		 EFM_ASSERT(test_struct.test_str[0][i] == test_struct.result_str[i]);
	 }

	 // What is this test validating?
	 // Student response:
	 // This first test validates that the length of the string pointed by the test
	 // is equal to the numerical value of test1_len.
	 EFM_ASSERT(strlen(test_struct.result_str) == test1_len);

	 // What is this test validating?
	 // Student response:
	 // The following EFM_ASSERT tests that an empty string has the same number of
	 // spaces as the defined value of CSIZE, which returns 64.
	 EFM_ASSERT(ble_circ_space() == CSIZE);

	 // What does this next push on the circular buffer test?
	 // Student Response:
	 // What happens if we push a string after a pop? What happens in a simple
	 // wrap-around case?

	 ble_circ_push(&test_struct.test_str[1][0]);


	 EFM_ASSERT(ble_circ_space() == (CSIZE - test2_len - 1));

	 // What does this next push on the circular buffer test?
	 // Student Response:
	 // The following tests popping two strings versus the above test, which is
	 // only one.
	 ble_circ_push(&test_struct.test_str[2][0]);


	 EFM_ASSERT(ble_circ_space() == (CSIZE - test2_len - 1 - test3_len - 1));

	 // What does this next push on the circular buffer test?
	 // Student Response:
	 EFM_ASSERT(abs(ble_cbuf.write_ptr - ble_cbuf.read_ptr) < CSIZE);

	 // Why is the expected buff_empty test = false?
	 // Student Response:
	 // ble_circ_pop as we've written is false as long as the circular buffer is
	 // empty.
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test2_len; i++){
		 EFM_ASSERT(test_struct.test_str[1][i] == test_struct.result_str[i]);
	 }

	 // What is this test validating?
	 // Student response:
	 // This test validates that he result of the test above spits out the same
	 // string length as the value of test 2
	 EFM_ASSERT(strlen(test_struct.result_str) == test2_len);

	 EFM_ASSERT(ble_circ_space() == (CSIZE - test3_len - 1));

	 // Why is the expected buff_empty test = false?
	 // Student Response:
	 // We want to make sure that the buffer is not empty,
	 // otherwise it will not return buff_empty.
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test3_len; i++){
		 EFM_ASSERT(test_struct.test_str[2][i] == test_struct.result_str[i]);
	 }

	 // What is this test validating?
	 // Student response:
	 // We are testing if the length of the resultant string is
	 // the same length as the 3rd test string length.
	 EFM_ASSERT(strlen(test_struct.result_str) == test3_len);

	 EFM_ASSERT(ble_circ_space() == CSIZE);

	 // Using these three writes and pops to the circular buffer, what other test
	 // could we develop to better test out the circular buffer?
	 // Student Response:


	 // Why is the expected buff_empty test = true?
	 // Student Response:
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == true);
	 ble_write("\nPassed Circular Buffer Test\n");

 }




