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
	GPIO gpio;

	while( 1 )
	{
		 gpio.PB3->toggle();
		        for(int i = 0; i < 100000; i++);  // Verzögerung
	}

	return 0;
}
