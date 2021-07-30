/*
 * ble.h
 *
 *  Created on: 10/22/20
 *      Author: Kay Sho
 *      Pronouns: (She/They)
 */
//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	BLE_GUARD_H
#define	BLE_GUARD_H

//** Standard Libraries
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Driver functions
#include "leuart.h"
#include "gpio.h"
#include "HW_delay.h"


//***********************************************************************************
// defined files
//***********************************************************************************

// BLE Module Additions
#define HM10_LEUART0		LEUART0
#define HM10_BAUDRATE		9600
#define	HM10_DATABITS		leuartDatabits8
#define HM10_ENABLE			leuartEnable
#define HM10_PARITY			leuartNoParity
#define HM10_REFFREQ		0					// use reference clock
#define HM10_STOPBITS		leuartStopbits1

#define LEUART0_TX_ENABLE	true
#define LEUART0_RX_ENABLE	true

// Circular Buffer addition
#define CIRC_TEST 			true
#define CIRC_OPER 			false
#define CSIZE 				128
#define CIRC_TEST_SIZE		3
#define CIRC_MN		 		1

// Circular Buffer Struct
typedef struct {
		char 		cbuf[CSIZE];
		uint8_t 	size_mask;
		uint32_t 	size;
		uint32_t 	read_ptr;
		uint32_t 	write_ptr;
} BLE_CIRCULAR_BUF;

// Test Circular Buffer Struct
typedef struct {
		char 		test_str[CIRC_TEST_SIZE][CSIZE];
		char 		result_str[CSIZE];
} CIRC_TEST_STRUCT;

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void ble_open(uint32_t tx_event, uint32_t rx_event);
void ble_write(char *string);

bool ble_test(char *mod_name);


void circular_buff_test(void);
bool ble_circ_pop(bool test);
#endif
