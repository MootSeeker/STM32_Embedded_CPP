/*
 * app.cpp
 *
 *  Created on: May 12, 2024
 *      Author: kevin
 */

#include "app.h"
#include "gpio.hpp"
#include "button.h"
#include "led.h"

uint8_t application_main( void )
{
	// LED-Objekt erstellen
	LED ledPin_PB3( GPIOB, 3 );

    // Button-Objekt erstellen
    Button button_PA7(GPIOA, 7);

	while( 1 )
	{
		ledPin_PB3.toggle( );
		for(int i = 0; i < 1000000; i++);  // Verzögerung
	}

	return 0;
}
