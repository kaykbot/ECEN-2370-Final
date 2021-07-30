//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef	APP_HG
#define	APP_HG

/* System include statements */
#include "stdint.h"
#include "string.h"
#include "stdio.h"

/* Silicon Labs include statements */
#include "em_cmu.h"
#include "em_assert.h"
#include "em_int.h"

/* The developer's include statements */
#include "cmu.h"
#include "gpio.h"
#include "letimer.h"
#include "brd_config.h"
#include "scheduler.h"
#include "sleep_routines.h"
#include "si7021.h"
#include "ble.h"
#include "HW_delay.h"
//***********************************************************************************
// defined files
//***********************************************************************************
#define		PWM_PER					2.7		// PWM period in seconds
#define		PWM_ACT_PER				0.15	// PWM active period in seconds
#define		TIMER_DELAY				2.0		// 2 second delay (no Magic Numbers)

// Application scheduled events
#define		LETIMER0_COMP0_CB		0x00000001	//0b0001
#define		LETIMER0_COMP1_CB		0x00000002	//0b0010
#define		LETIMER0_UF_CB			0x00000004	//0b0100
#define		BOOT_UP_CB				0x00000010	//0b100

// Si7021 scheduled events:
#define 	SI7021_READ_DONE_CB		0x00000008	//0b1000

// LEUART Addition:
#define		SYSTEM_BLOCK_EM			EM3
#define 	BLE_RX_DONE_CB			0x00000020
#define		BLE_TX_DONE_CB			0x00000040
#define 	IMPERIAL				true
#define		METRIC					false
//***********************************************************************************
// global variables
//***********************************************************************************
char buffer[50];


//***********************************************************************************
// function prototypes
//***********************************************************************************
void app_peripheral_setup(void);
void scheduled_letimer0_comp0_cb(void);
void scheduled_letimer0_comp1_cb(void);
void scheduled_letimer0_uf_cb(void);

void scheduled_boot_up_cb(void);

void scheduled_tx_done_cb(void);
void scheduled_rx_done_cb(void);
void scheduled_si7021_read_done_cb(void);
#endif
