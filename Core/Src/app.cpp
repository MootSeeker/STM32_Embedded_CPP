/*
 * app.cpp
 *
 *  Created on: May 12, 2024
 *      Author: kevin
 */

#include "app.h"
#include "pins.h"

  #define LED_BANK (GPIOB)
  #define LED_PIN  (5)

bGPIO     led_gpio;
bGPIO_PIN board_led;

uint8_t application_main( void )
{
	// Initialize the LED pin's GPIO bank.
	  led_gpio = bGPIO(LED_BANK);

	  // Initialize the LED pin.
	  board_led = bGPIO_PIN(&led_gpio, LED_PIN, pGPIO_OUT_PP);


	while( 1 )
	{
		board_led.toggle();
		HAL_Delay( 500 );
	}

	return 0;
}
