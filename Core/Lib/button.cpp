/*
 * button.cpp
 *
 *  Created on: May 16, 2024
 *      Author: kevin.perillo
 */




#include "button.h"

Button::Button( GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin ) : GPIO( GPIOx, GPIO_Pin )
{
	configure_button( NO_PULL );
}

void Button::configure_button( GPIO::Pull pull )
{
	configure( INPUT, pull, LOW );
	configure_exti( );
}

void Button::configure_exti( )
{

}

// IRQ-Handler-Funktion
extern "C" void EXTI4_15_IRQHandler( void )
{
	if( EXTI->PR1 & ( 1 << 7 ) )
	{ // Überprüfen, ob das Ereignis von Pin PA7 kommt
		EXTI->PR1 = ( 1 << 7 ); // Clear pending bit
		// Cast to Button type and call exti_callback
		reinterpret_cast<Button*>( GPIOA )->exti_callback( );
	}
}
