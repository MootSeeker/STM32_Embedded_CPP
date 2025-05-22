/*
 * gpio.cpp
 *
 *  Created on: May 22, 2024
 *      Author: kevin.perillo
 */


#include "gpio.h"

// Konstruktor: Hier wird die Basisadresse des GPIO-Ports gesetzt
GPIO::GPIO(Port port, uint8_t pin) : pin(pin), portBase([port]() -> GPIO_TypeDef* {
    switch (port) {
        case Port::PORTA:
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
            return reinterpret_cast<GPIO_TypeDef*>(GPIOA_BASE);
        case Port::PORTB:
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
            return reinterpret_cast<GPIO_TypeDef*>(GPIOB_BASE);
        case Port::PORTC:
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
            return reinterpret_cast<GPIO_TypeDef*>(GPIOC_BASE);
#ifdef RCC_AHB2ENR_GPIODEN
        case Port::PORTD:
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
            return reinterpret_cast<GPIO_TypeDef*>(GPIOD_BASE);
#endif
#ifdef RCC_AHB2ENR_GPIOEEN
        case Port::PORTE:
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
            return reinterpret_cast<GPIO_TypeDef*>(GPIOE_BASE);
#endif
#ifdef RCC_AHB2ENR_GPIOFEN
        case Port::PORTF:
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOFEN;
            return reinterpret_cast<GPIO_TypeDef*>(GPIOF_BASE);
#endif
#ifdef RCC_AHB2ENR_GPIOGEN
        case Port::PORTG:
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;
            return reinterpret_cast<GPIO_TypeDef*>(GPIOG_BASE);
#endif
        case Port::PORTH:
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOHEN;
            return reinterpret_cast<GPIO_TypeDef*>(GPIOH_BASE);
        default:
            return nullptr;
    }
}()) {}


// Diese Methode verwendet die Basisadresse, um den Pin-Modus zu setzen
void GPIO::setMode( Mode mode )
{
	portBase->MODER &= ~( 0x3 << ( pin * 2 ) );
	switch ( mode )
	{
		case Mode::INPUT:
			portBase->MODER |= ( 0x0 << ( pin * 2 ) );
		break;
		case Mode::OUTPUT:
			portBase->MODER |= ( 0x1 << ( pin * 2 ) );
		break;
		case Mode::ALTERNATE:
			portBase->MODER |= ( 0x2 << ( pin * 2 ) );
		break;
		case Mode::ANALOG:
		default:	// Default case is analog configuration to save power
			portBase->MODER |= ( 0x3 << ( pin * 2 ) );
		break;
	}
}

void GPIO::setPull( Pull pull )
{
	portBase->PUPDR &= ~( 0x3 << ( pin * 2 ) );
	switch ( pull )
	{
		case Pull::PULL_UP:
			portBase->PUPDR |= ( 0x1 << ( pin * 2 ) );
		break;
		case Pull::PULL_DOWN:
			portBase->PUPDR |= ( 0x2 << ( pin * 2 ) );
		break;
		case Pull::NO_PULL:
		default:	// Default case should also be no pullup
			portBase->PUPDR |= ( 0x0 << ( pin * 2 ) );
		break;
	}
}

void GPIO::setSpeed( Speed speed )
{
	portBase->OSPEEDR &= ~( 0x3 << ( pin * 2 ) );
	switch ( speed )
	{
		case Speed::MEDIUM:
			portBase->OSPEEDR |= ( 0x1 << ( pin * 2 ) );
		break;
		case Speed::HIGH:
			portBase->OSPEEDR |= ( 0x2 << ( pin * 2 ) );
		break;
		case Speed::VERY_HIGH:
			portBase->OSPEEDR |= ( 0x3 << ( pin * 2 ) );
		break;
		case Speed::LOW:
		default: 	// Default case is low
			portBase->OSPEEDR |= ( 0x0 << ( pin * 2 ) );
		break;
	}
}

void GPIO::setAlternateFunction( AlternateFunction af )
{
	if( pin < 8 )
	{
		portBase->AFR[ 0 ] &= ~( 0xF << ( pin * 4 ) );
		portBase->AFR[ 0 ] |= ( static_cast<uint32_t>( af ) << ( pin * 4 ) );
	}
	else
	{
		portBase->AFR[ 1 ] &= ~( 0xF << ( ( pin - 8 ) * 4 ) );
		portBase->AFR[ 1 ] |= ( static_cast<uint32_t>( af ) << ( ( pin - 8 ) * 4 ) );
	}
}

void GPIO::write( bool value )
{
	if( value )
	{
		portBase->BSRR = getPinMask( );
	}
	else
	{
		portBase->BRR = getPinMask( );
	}
}

bool GPIO::read( )
{
	return ( portBase->IDR & getPinMask( ) ) != 0;
}

uint32_t GPIO::getPinMask( ) const
{
	return 1 << pin;
}

