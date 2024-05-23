/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <common.h>
#include <app.h>
#include <rcc.h>
#include <gpio.h>



int main( void )
{
	CLOCK CLOCK;

    // Enable HSI clock
	CLOCK.enableHSI();

    // Configure PLL
	CLOCK.configurePLL();

    // Enable PLL
	CLOCK.enablePLL();

    // Select PLL as system clock
	CLOCK.selectSystemClock();

	GPIO led( GPIO::Port::PORTB, 3 ); // LED an PB0
	led.setMode( GPIO::Mode::OUTPUT );
	led.setPull( GPIO::Pull::NO_PULL );
	led.setSpeed( GPIO::Speed::VERY_HIGH );

	while( 1 )
	{
		led.write( true );  // LED an
		for(  int i = 0; i < 100000; i++ ); // Wartezeit
		led.write( false ); // LED aus
		for(  int i = 0; i < 100000; i++ ); // Wartezeit
	}
}
