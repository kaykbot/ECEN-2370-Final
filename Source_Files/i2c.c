/**
 * @file i2c.c
 * @author Kay Sho
 * @date  10/1/2020
 * @brief Contains the driver functions for the i2c peripheral
 * @note The author's pronouns: (She/They)
 * @note In order for the doxygen to appear, there must be an "at file"
 */
//***********************************************************************************
// Include files
//***********************************************************************************

/* System include statements */


/* Silicon Labs include statements */


/* The developer's include statements */
#include "i2c.h"



//***********************************************************************************
// defined files
//***********************************************************************************
#define I2C_FREQ_FAST_MAX   392157 // Because the Si7021 works on 400kHz, we use the FAST

//***********************************************************************************
// private variables
//***********************************************************************************
static void i2c_bus_reset(I2C_TypeDef *i2c);

static void i2c_ack(void); // functions contain states
static void i2c_nack(void);
static void i2c_rxdatav(void);
static void i2c_mstop(void);

typedef enum{
		IDLE,
		REQUEST_DEVICE,
		WRITE_DEVICE,
		WAIT_CONVERSION,
		READ_DEVICE,
		CLOSE
}I2C_STATE;


typedef struct{
	I2C_TypeDef* 			i2c;				// I2C Peripheral
	uint8_t 				dev_addr; 			// I2C Device Address (Si7021)
	uint8_t					reg_addr;			// I2C Requested Register address (also known as command address?)
	bool					rw_mode;			// Read 1/Write 0
	uint32_t*				sm_data;			// Pointer to where to store read result / write data
	uint8_t					sm_data_len;
	uint8_t					transfer_bytes;		// how many bytes to transfer
	uint32_t				event;
	I2C_STATE				state;			// State

}I2C_STATE_MACHINE;

static I2C_STATE_MACHINE i2c_sm;
//***********************************************************************************
// Global functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 * Initializes the I2C Peripheral
 *
 * @details
 * recall si7021 calls the i2c_Start function
 * @note
 *
 * @param[in] *i2c
 *
 * Our defined I2C struct
 *
 * @param[in] *i2c_setup
 *
 * Our i2c struct defined in i2c.h
 *
 ******************************************************************************/
void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *i2c_setup)

{
	if (i2c == I2C0) // Establish what I2C peripheral we're using
	CMU_ClockEnable(cmuClock_I2C0, true);
	else if(i2c == I2C1)
		CMU_ClockEnable(cmuClock_I2C1, true);
	if ((i2c->IF & 0x01) == 0)
	{
	i2c->IFS = 0x01;
	EFM_ASSERT(i2c->IF & 0x01);
	i2c->IFC = 0x01;
	}
	else {
	i2c->IFC = 0x01;
	EFM_ASSERT(!(i2c->IF & 0x01));
	}

	I2C_Init_TypeDef i2c_init_values; 	// Struct that holds all values that goes into init function
	// The TypeDef is already defied in em.i2c.h

	// I2C Bus Initialization

	i2c_init_values.enable = i2c_setup->enable;
	i2c_init_values.master = true;
	i2c_init_values.refFreq = 0 ;
	i2c_init_values.freq = I2C_FREQ_FAST_MAX; //Check Datasheet/reference manual?
	i2c_init_values.clhr = i2cClockHLRAsymetric;

	I2C_Init(i2c, &i2c_init_values);

	// Below code routes SCL & SDA pins (Lines 63-71)
	i2c->ROUTELOC0 |= i2c_setup -> scl_pin_route;
	i2c->ROUTELOC0 |= i2c_setup -> sda_pin_route;

	if (i2c_setup -> scl_pin_en)
	{i2c->ROUTEPEN |= I2C_ROUTEPEN_SCLPEN;}
	if (i2c_setup->sda_pin_en)
	{i2c->ROUTEPEN |= I2C_ROUTEPEN_SDAPEN;}

	// Setting interrupt flag bits:

	uint32_t i2c_interrupts = I2C_IEN_ACK | I2C_IEN_NACK | I2C_IEN_RXDATAV | I2C_IEN_MSTOP;

	//Interrupts + NVIC

	I2C_IntClear(i2c, i2c_interrupts);

	I2C_IntEnable(i2c, i2c_interrupts);

	if(i2c == I2C0){
		NVIC_EnableIRQ(I2C0_IRQn);
	} else if (i2c == I2C1){
		NVIC_EnableIRQ(I2C1_IRQn);
	} else {
		EFM_ASSERT(false);
	}
	i2c_bus_reset(i2c);
	i2c_sm.state = IDLE;
}

/***************************************************************************//**
 * @brief
 *
 * Function that starts the I2C bus
 *
 * @details
 *
 * recall si7021 calls the i2c_Start function
 *
 * @note
 *
 * All of the parameters can be found in the i2c.h header file, in the state machine struct
 *
 * @param[in] *i2c
 * built in struct for the i2c
 *
 * @param[in] dev_addr
 * Defined Device address (In this case the Si7021 Temperature Sensor
 *
 * @param[in] rw_mode
 * input to indicate whether in read or write mode
 *
 * @param[in] reg_addr
 * Register address (AKA Command address) is the input for the Si7021 command address
 *
 * @param[in] sm_data
 * This is the incoming data from the state machine
 *
 * @param[in] sm_data_len
 * state machine Data array length
 *
 * @param[in]  event
 * Scheduled event defined and called in app.c/.h
 *
 * @param[in] enable
 * Enables i2c start (unnecessary, can remove later)
 ******************************************************************************/
void i2c_start(I2C_TypeDef *i2c, uint8_t dev_addr, bool rw_mode, uint8_t reg_addr, uint32_t* sm_data, uint8_t sm_data_len, uint32_t event, bool enable)

{
	EFM_ASSERT((i2c->STATE & _I2C_STATE_STATE_MASK) == I2C_STATE_STATE_IDLE);

	sleep_block_mode(I2C_EM_BLOCK);

	i2c_sm.i2c = i2c;
	i2c_sm.dev_addr = dev_addr;
	i2c_sm.reg_addr = reg_addr;
	i2c_sm.rw_mode = I2C_READ;
	i2c_sm.sm_data = sm_data;
	i2c_sm.sm_data_len = sm_data_len;
	i2c_sm.transfer_bytes = 0;
	i2c_sm.state = REQUEST_DEVICE;
	i2c_sm.event = event;


	i2c_sm.i2c->CMD = I2C_CMD_START; // Set start bit (?)
	i2c_sm.i2c->TXDATA = (i2c_sm.dev_addr << 1) | I2C_WRITE; //(TXData: read/write?)
}

/***************************************************************************//**
 * @brief
 *
 * 		IRQ Handler for I2C0
 *
 * @details
 *
 * 		This function sets up the Interrupt Service Routine for the I2C
 *
 * @note
 *
 *		Just like how the IRQHandler function for the LETIMER has cases for  Comp0, Comp1 and UF,
 *		The I2C0 IRQHandler function will handle it's own cases. These are as determined in the
 *		software ladder flowchart:
 *		> ACK
 *		> NACK
 *		> RXDATAV (Received data value)
 *		> MSTOP
 *
 *
 ******************************************************************************/
void I2C0_IRQHandler(void){

	uint32_t int_flag;
	int_flag = I2C0->IF & I2C0->IEN; 	// We can also use the I2C_IntXYZ functions
	I2C0->IFC = int_flag;				// I2C_IntGet/Enabled/Clear

	if(int_flag & I2C_IF_ACK)
	{i2c_ack();}

	if(int_flag & I2C_IF_NACK)
	{i2c_nack();}

	if(int_flag & I2C_IF_RXDATAV)
	{i2c_rxdatav();}

	if(int_flag & I2C_IF_MSTOP)
	{i2c_mstop();}
}
/***************************************************************************//**
 * @brief
 * Sets up the possible cases for the Acknowledge Interrupt
 *
 * @details
 *
 * In the case that an ACK is detected, the different cases of the ACK switch statement are defined
 *
 * @note
 *
 * WAIT_CONVERSION case is equivalent to what others call 'Receive MSB'
 * READ_DEVICE case is equivalent to what others call 'Receive LSB'
 *
 ******************************************************************************/
static void i2c_ack(){

	switch(i2c_sm.state){
		case IDLE:
			EFM_ASSERT(false);
			break;
		case REQUEST_DEVICE: // "requesting device" to send measurement
			i2c_sm.state = WRITE_DEVICE;
			i2c_sm.i2c->TXDATA = i2c_sm.reg_addr;
			break;
		case WRITE_DEVICE:
			if(i2c_sm.rw_mode)
			{
				i2c_sm.state = WAIT_CONVERSION;
				i2c_sm.i2c->CMD = I2C_CMD_START;
				i2c_sm.i2c->TXDATA = (i2c_sm.dev_addr << 1) | I2C_READ;
			}
			break;
		case WAIT_CONVERSION:
			i2c_sm.state = READ_DEVICE;
			break;
		case READ_DEVICE:
			EFM_ASSERT(false);
			break;
		case CLOSE:
			EFM_ASSERT(false);
			break;
		default:
			EFM_ASSERT(false);
			break;
	}
}
/***************************************************************************//**
 * @brief
 * NACK case statement
 *
 * @details
 *
 * In the case that a NACK is detected, the following states will do as such.
 *
 * @note
 *
 * Only WAIT_CONVERSION houses actionable code.
 *
 ******************************************************************************/

static void i2c_nack(){
	uint8_t data;
	switch(i2c_sm.state){
		case IDLE:
			EFM_ASSERT(false);
			break;
		case REQUEST_DEVICE:
			EFM_ASSERT(false);
			break;
		case WRITE_DEVICE:
			EFM_ASSERT(false);
			break;
		case WAIT_CONVERSION:
			if(i2c_sm.rw_mode)
			{
				i2c_sm.i2c->CMD = I2C_CMD_START;
				data =  (i2c_sm.dev_addr << 1) | I2C_READ;
				i2c_sm.i2c->TXDATA = data;
			}
			break;
		case READ_DEVICE:
			EFM_ASSERT(false);
			break;
		case CLOSE:
			EFM_ASSERT(false);
			break;
		default:
			EFM_ASSERT(false);
			break;
	}
}

/***************************************************************************//**
 * @brief
 *
 * RXDATAV case statement
 *
 * @details
 *
 * Establishes all of the cases for the RXDATAV interrupt.
 *
 * @note
 *
 * Actionable code is located in WAIT_CONVERSION and READ_DEVICE cases.
 *
 ******************************************************************************/

static void i2c_rxdatav(){
	switch(i2c_sm.state){
		case IDLE:
			EFM_ASSERT(false);
			break;

		case REQUEST_DEVICE:
			EFM_ASSERT(false);
			break;

		case WRITE_DEVICE:
			EFM_ASSERT(false);
			break;
		case WAIT_CONVERSION: // "Receive_MSB"
			i2c_sm.state = READ_DEVICE; //State change
			i2c_sm.sm_data[i2c_sm.transfer_bytes] = i2c_sm.i2c->RXDATA; // Read RXDATA register from i2c
			i2c_sm.i2c->CMD = I2C_CMD_ACK;		//Send ACK command to PG12 sensor
			break;

		case READ_DEVICE: // "Receive_LSB"
			i2c_sm.state = CLOSE;
			i2c_sm.sm_data[i2c_sm.transfer_bytes] = i2c_sm.i2c->RXDATA;
				i2c_sm.i2c->CMD = I2C_CMD_NACK;
				i2c_sm.i2c->CMD = I2C_CMD_STOP;
			break;
		case CLOSE:
			EFM_ASSERT(false);
			break;
		default:
			EFM_ASSERT(false);
			break;
	}
}
/***************************************************************************//**
 * @brief
 *	Case for receiving I2C MSTOP Interrupt
 *
 * @details
 *
 * Sets up the state machine for when the case for when the [Manager] STOP condition is set
 *
 * @note
 *
 * While not standard standard terminology, my code will use Manager/Subscriber terminology in place of Master/Slave
 *
 ******************************************************************************/
static void i2c_mstop(){
	switch(i2c_sm.state){
		case IDLE:
			EFM_ASSERT(false);
			break;
		case REQUEST_DEVICE: // "requesting device" to send measurement
			EFM_ASSERT(false);
			break;
		case WRITE_DEVICE:
			EFM_ASSERT(false);
			break;
		case WAIT_CONVERSION:
			EFM_ASSERT(false);
			break;
		case READ_DEVICE:
			EFM_ASSERT(false);
			break;
		case CLOSE:
			i2c_sm.state = IDLE;	//setting I2C State to IDLE
			sleep_unblock_mode(I2C_EM_BLOCK); //Recall that I2C_EM_BLOCK is EM2
			add_scheduled_event(i2c_sm.event);// Set event for Si7021 Read
			break;
		default:
			EFM_ASSERT(false);
			break;
	}
}

//***********************************************************************************
// Private functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *
 * Contains the I2C Bus reset function
 *
 * @details
 *
 * Resets/clears the states of the I2C Bus, clearing enabled interrupts and interrupt flags.
 *
 * @note
 *
 *
 * @param[in] *i2c
 *
 * Requires values from the I2C library.
 *
 ******************************************************************************/
static void i2c_bus_reset(I2C_TypeDef *i2c){
		uint32_t int_flag;
		uint32_t int_en;

		int_en = i2c->IEN; 								// Check that the IEN register is clear + saving state of IEN
		int_flag = i2c->IF;								// Check if IF is clear
		i2c->IEN &= ~int_en;							// Disables interrupts
		i2c->IFC = int_flag;							// Clear the interrupt flag register
		i2c -> CMD = I2C_CMD_CLEARTX; 					// Clear the transmit buffer
		i2c->CMD = I2C_CMD_START | I2C_CMD_STOP; 		// Set the start and stop bits in the CMD register
		while(!(i2c->IF & I2C_IF_MSTOP));				// Stall until the stop command completes
		i2c->IFC = I2C_IFC_START | I2C_IFC_MSTOP;		// Clear the interrupt flag register again
		i2c->IEN = int_en;								// Reloading state of IEN
		i2c->CMD = I2C_CMD_ABORT;						// Reset the i2c peripheral state machine
}
