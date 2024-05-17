/*
 * gpio.cpp
 *
 *  Created on: May 16, 2024
 *      Author: kevin.perillo
 */



#include "gpio.hpp"

GPIO::GPIO( GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin ) :
		port( GPIOx ), pin( GPIO_Pin ), mode( INPUT ), pull( NO_PULL ), speed( LOW ), alternate( 0 )
{
	enable_gpio_clock( );
}

void GPIO::configure( Mode mode, Pull pull, Speed speed, uint8_t alternate )
{
	this->mode = mode;
	this->pull = pull;
	this->speed = speed;
	this->alternate = alternate;

	// Modus einstellen
	port->MODER &= ~( 0x3 << ( 2 * pin ) );
	port->MODER |= ( mode << ( 2 * pin ) );

	// Pull-Up/Pull-Down einstellen
	port->PUPDR &= ~( 0x3 << ( 2 * pin ) );
	port->PUPDR |= ( pull << ( 2 * pin ) );

	// Geschwindigkeit einstellen
	port->OSPEEDR &= ~( 0x3 << ( 2 * pin ) );
	port->OSPEEDR |= ( speed << ( 2 * pin ) );

	// Alternate Function einstellen, falls erforderlich
	if( mode == ALTERNATE )
	{
		if( pin < 8 )
		{
			port->AFR[ 0 ] &= ~( 0xF << ( 4 * pin ) );
			port->AFR[ 0 ] |= ( alternate << ( 4 * pin ) );
		}
		else
		{
			port->AFR[ 1 ] &= ~( 0xF << ( 4 * ( pin - 8 ) ) );
			port->AFR[ 1 ] |= ( alternate << ( 4 * ( pin - 8 ) ) );
		}
	}
}

void GPIO::write( uint32_t data )
{
	if( data )
	{
		port->BSRR = ( 1 << pin );
	}
	else
	{
		port->BRR = ( 1 << pin );
	}
}

unsigned GPIO::read( ) const
{
	return ( port->IDR & ( 1 << pin ) ) ? 1 : 0;
}

void GPIO::stream( uint32_t *buffer )
{

}

void GPIO::toggle( )
{
	port->ODR ^= ( 1 << pin );
}

void GPIO::lock( )
{
	port->LCKR = ( 1 << pin ) | GPIO_LCKR_LCKK;
	port->LCKR = ( 1 << pin );
	port->LCKR = ( 1 << pin ) | GPIO_LCKR_LCKK;
	( void ) port->LCKR; // Dummy read to complete the locking process
}

void GPIO::handle_exti_irq( )
{
	if( EXTI->PR1 & ( 1 << pin ) )
	{
		EXTI->PR1 = ( 1 << pin ); // Clear the pending bit
		exti_callback( );
	}
}

void GPIO::exti_callback( )
{

}

void GPIO::enable_gpio_clock( )
{
	if( port == GPIOA )
	{
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	}
	else if( port == GPIOB )
	{
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	}
	else if( port == GPIOC )
	{
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	}
}
