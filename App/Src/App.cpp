/**
 * Filename: App.cpp
 */

#include "App.h"
#include "main.h"

#include "gpio.h"

#include <cstdio>

using namespace GPIO;


/**
 * @brief EXTI Line 0 interrupt handler
 *
 * Handles interrupts for GPIO pins connected to EXTI line 0.
 * Calls the appropriate GPIO object's interrupt handler.
 */
extern "C" void EXTI0_IRQHandler(void)
{
    // Handle button 2 (PA0) interrupt
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_0)) {
        GPIO::GPIOEXTI::handleInterrupt(0);
    }
}

void btn2InterruptCallback( )
{

}

void App_Init( void )
{

	GPIOOutput led( GPIOB, 11, PinOutputType::PUSH_PULL );

	GPIOEXTI btn1( GPIOC, 0, EXTITrigger::FALLING, PinPull::PULL_UP );
	GPIOEXTI btn2( GPIOC, 1, EXTITrigger::FALLING, PinPull::PULL_UP );


    printf("App_init: Initializing application...\n");

    led.reset();

    btn2.setCallback( btn2InterruptCallback );

    btn2.enableInterrupt( );

    while( 1) {}
}

void App_Run( void )
{
    printf("App_start: Main application task started\n");





	while( true )
	{

	}
}
