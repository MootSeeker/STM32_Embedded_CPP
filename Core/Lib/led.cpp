/*
 * led.cpp
 *
 *  Created on: May 16, 2024
 *      Author: kevin.perillo
 */


#include "led.h"


LED::LED( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin ) : GPIO( GPIOx, GPIO_Pin )
{
	// Set port and pin
	this->port = GPIOx;
	this->pin = GPIO_Pin;

	// Set io configuration
	configure_led( NO_PULL );
}

void LED::configure_led( GPIO::Pull pull )
{
    configure(OUTPUT, pull, LOW);
    this->value = LOW;
}

void LED::toggle( )
{
    this->value = !this->value;
    this->port->ODR ^= (1 << this->pin);  // Toggle den Pin im ODR-Register
}
