/**
 * @file    App.cpp
 * @brief   Basic GPIO Blinky Example
 */

#include "App.h"
#include "main.h"
#include "gpio.h"

using namespace GPIO;

// Global pointer to the LED object
static GPIOOutput* led = nullptr;

/**
 * @brief Application Initialization
 * 
 * Called once before the main loop.
 * setup hardware objects here.
 */
void App_Init( void )
{
    // Configure PB11 as Output (LED)
    // Note: Use 'new' only in Init phase
    led = new GPIOOutput( GPIOB, LL_GPIO_PIN_11 );
}

/**
 * @brief Application Main Loop
 * 
 * Contains the infinite loop for the application.
 */
void App_Run( void )
{
    while ( 1 )
    {
        if ( led != nullptr )
        {
            led->toggle( );
        }

        // Toggle every 500ms
        HAL_Delay( 500 );
    }
}
