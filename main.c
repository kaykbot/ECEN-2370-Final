
#include "main.h"

int main(void){
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with kit specific parameters */
  /* Init DCDC regulator and HFXO with kit specific parameters */
  /* Initialize DCDC. Always start in low-noise mode. */
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  EMU_EM23Init(&em23Init);
  CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFRCO and disable HFRCO */
  CMU_HFRCOBandSet(MCU_HFXO_FREQ);					// Set main CPU oscillator frequency
  CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
  CMU_OscillatorEnable(cmuOsc_HFXO, false, false);

  /* Call application program to open / initialize all required peripheral */
  app_peripheral_setup();

  /* Infinite blink loop */
  while (1) {
	  	  CORE_DECLARE_IRQ_STATE;
	  	  CORE_ENTER_CRITICAL();
	  if (!get_scheduled_events()) enter_sleep();
	  	  CORE_EXIT_CRITICAL();

	  if(get_scheduled_events() & LETIMER0_COMP0_CB)
	  {scheduled_letimer0_comp0_cb();}

	  if(get_scheduled_events() & LETIMER0_COMP1_CB)
	  {scheduled_letimer0_comp1_cb();}

	  if(get_scheduled_events() & LETIMER0_UF_CB)
	  {scheduled_letimer0_uf_cb();}

	  if(get_scheduled_events() & SI7021_READ_DONE_CB)
	  {scheduled_si7021_read_done_cb();}

	  if(get_scheduled_events() & BOOT_UP_CB)
	  {scheduled_boot_up_cb();}

	  if(get_scheduled_events() & BLE_TX_DONE_CB)
	  {scheduled_tx_done_cb();}

	  if(get_scheduled_events() & BLE_RX_DONE_CB)
	  {scheduled_rx_done_cb();}
  }
}
