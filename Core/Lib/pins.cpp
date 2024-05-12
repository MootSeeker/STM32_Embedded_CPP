/*
 * pins.cpp
 *
 *  Created on: May 12, 2024
 *      Author: kevin
 */

#include "pins.h"

bGPIO::bGPIO( ) { }

bGPIO::bGPIO( GPIO_TypeDef* port )
{
	switch( port )
	{
		case GPIOA:
		{
			__HAL_RCC_GPIOA_CLK_ENABLE( );
			break;
		}

		case GPIOB:
		{
			__HAL_RCC_GPIOB_CLK_ENABLE( );
			break;
		}

		case GPIOC:
		{
			__HAL_RCC_GPIOC_CLK_ENABLE( );
			break;
		}

		case GPIOH:
		{
			__HAL_RCC_GPIOH_CLK_ENABLE( );
			break;
		}

		default:
		{
			this->status = HAL_ERROR;
			break;
		}
	}

	// TODO: Add #ifdef for larger Chips GPIOD etc.
	this->status = HAL_OK;
}




// Common read / write methods from Base class
unsigned bGPIO::read( void )
{
	if( HAL_ERROR != this->status )
	{
		return this->gpio->IDR;
	}
	else
	{
		return 0;
	}
}


void bGPIO::write( unsigned data )
{
	if( HAL_ERROR != this->status )
	{
		// Write whole port when initialized
		this->gpio->ODR = data & 0xFFFF;
	}
}

    /**
     * @brief Stream data.
     * @param buffer Pointer to the data buffer.
     * @param length Length of the buffer.
     */
void bGPIO::stream( volatile void *buffer, int length )
{
   	volatile uint32_t* gpiobuf = (volatile uint32_t*) buf;

   	if (HAL_ERROR != this->status )
   	{
   		// Write all of the values, one by one.
   		for (int si = 0; si < len; ++si)
    	{
    		write(gpiobuf[si]);
    	}
   	}
}

    // Generic GPIO methods
GPIO_PinState bGPIO::read_pin(unsigned pin_num)
{
	if( HAL_ERROR != this->status )
	{
		// Return masked bit in IDR register
		return ( this->gpio->IDR & ( 1 << pin_num ));
	}
	else
	{
		return 0;
	}
}

void bGPIO::pin_on(unsigned pin_num)
{
	if( HAL_ERROR != this->status )
	{
		this->gpio->ODR |= ( 1 << pin_num );
	}
}

void bGPIO::pin_off( unsigned pin_num )
{
	if( HAL_ERROR != this->status )
	{
		this->gpio->ODR &= ~( 1 << pin_num );
	}
}

void bGPIO::pin_toggle(unsigned pin_num)
{
	if( HAL_ERROR != this->status )
	{
		this->gpio->ODR ^= ( 1 << pin_num );
	}
}

	// GPIO class specific methods

void bGPIO::set_mode( unsigned pin, unsigned mode )
{
	if( HAL_ERROR != this->status )
	{
		this->gpio->MODER &= ~(3 << (pin * 2));
		this->gpio->MODER |=  (mode << (pin * 2));
	}
}

void bGPIO::type( unsigned pin, unsigned type )
{
	if( HAL_ERROR != this->status )
	{
		this->gpio->OTYPER &= ~(1 << pin);
		this->gpio->OTYPER |=  (pull_type << pin);
	}
}

void bGPIO::set_pull( unsigned pin, unsigned pull_type )
{
	if( HAL_ERROR != this->status )
	{
		this->gpio->PUPDR &= ~(3 << (pin * 2));
		this->gpio->PUPDR |=  (pull_type << (pin * 2));
	}
}

void bGPIO::set_speed( unsigned pin, unsigned speed )
{
	if( HAL_ERROR != this->status )
	{
		this->gpio->OSPEEDR &= ~(3 << (pin * 2));
		this->gpio->OSPEEDR |=  (speed << (pin * 2));
	}
}

void bGPIO::set_af( unsigned pin, unsigned af )
{
	if( HAL_ERROR != this->status )
	{
		if (pin < 8)
		{
		  gpio->AFR[0] &= ~(0xF << (pin * 4));
		  gpio->AFR[0] |=  (af  << (pin * 4));
		}
		else
		{
		  gpio->AFR[1] &= ~(0xF << ((pin - 8) * 4));
		  gpio->AFR[1] |=  (af  << ((pin - 8) * 4));
	  }
	}
}
