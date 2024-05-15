/*
 * app.cpp
 *
 *  Created on: May 12, 2024
 *      Author: kevin
 */

#include "app.h"
#include "gpio.hpp"

uint8_t application_main( void )
{

    // Erstellen Sie ein GPIO-Objekt für Port A
    GPIO gpioA(GPIOB);

    // Konfigurieren Sie Pin 0 als Ausgang
    gpioA.pins[3]->configure(PinMode::OUTPUT);

    // Setzen Sie Pin 0 auf HIGH
    gpioA.pins[3]->write(true);



	while( 1 )
	{
		gpioA.pins[3]->toggle();
		for(int i = 0; i < 1000000; i++);  // Verzögerung
	}

	return 0;
}
