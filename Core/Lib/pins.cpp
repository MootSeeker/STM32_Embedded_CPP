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
	int sel = 0xFFFF;

	if( GPIOA == port ) sel = 0;
	else if ( GPIOB == port ) sel = 1;
	else if ( GPIOC == port ) sel = 2;
	else if ( GPIOH == port ) sel = 3;
	else sel = 0xFFFF;

	switch( sel )
	{
		case 0:
		{
			__HAL_RCC_GPIOA_CLK_ENABLE( );
			break;
		}

		case 1:
		{
			__HAL_RCC_GPIOB_CLK_ENABLE( );
			break;
		}

		case 2:
		{
			__HAL_RCC_GPIOC_CLK_ENABLE( );
			break;
		}

		case 3:
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
		return gpio->IDR;
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
		gpio->ODR = data & 0xFFFF;
	}
}

    /**
     * @brief Stream data.
     * @param buffer Pointer to the data buffer.
     * @param length Length of the buffer.
     */
void bGPIO::stream( volatile void *buffer, int length )
{
   	volatile uint32_t* gpiobuf = (volatile uint32_t*) buffer;

   	if (HAL_ERROR != this->status )
   	{
   		// Write all of the values, one by one.
   		for (int si = 0; si < length; ++si)
    	{
    		write(gpiobuf[si]);
    	}
   	}
}

    // Generic GPIO methods
int bGPIO::read_pin(unsigned pin_num)
{
	if( HAL_ERROR != this->status )
	{
		// Return masked bit in IDR register
		return ( gpio->IDR & ( 1 << pin_num ));
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
		gpio->ODR |= ( 1 << pin_num );
	}
}

void bGPIO::pin_off( unsigned pin_num )
{
	if( HAL_ERROR != this->status )
	{
		gpio->ODR &= ~( 1 << pin_num );
	}
}

void bGPIO::pin_toggle(unsigned pin_num)
{
	if( HAL_ERROR != this->status )
	{
		gpio->ODR ^= ( 1 << pin_num );
	}
}

	// GPIO class specific methods

void bGPIO::set_mode( unsigned pin, unsigned mode )
{
	if( HAL_ERROR != this->status )
	{
		gpio->MODER &= ~(3 << (pin * 2));
		gpio->MODER |=  (mode << (pin * 2));
	}
}

void bGPIO::set_type( unsigned pin, unsigned type )
{
	if( HAL_ERROR != this->status )
	{
		gpio->OTYPER &= ~(1 << pin);
		gpio->OTYPER |=  (type << pin);
	}
}

void bGPIO::set_pull( unsigned pin, unsigned pull_type )
{
	if( HAL_ERROR != this->status )
	{
		gpio->PUPDR &= ~(3 << (pin * 2));
		gpio->PUPDR |=  (pull_type << (pin * 2));
	}
}

void bGPIO::set_speed( unsigned pin, unsigned speed )
{
	if( HAL_ERROR != this->status )
	{
		gpio->OSPEEDR &= ~(3 << (pin * 2));
		gpio->OSPEEDR |=  (speed << (pin * 2));
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


// GPIO Pin Class ..................................................................
/* GPIO Pin class methods. */
// Default constructor.
bGPIO_PIN::bGPIO_PIN() {}

// Convenience constructor.
bGPIO_PIN::bGPIO_PIN(bGPIO* pin_bank, uint8_t pin_num, pGPIO_pin_qinit q) {
  // Set basic values.
  bank = pin_bank;
  pin = pin_num;
  // Set the pin registers according to the quick reference.
  // Different registers for different lines of chips.
    switch(q)
    {
      case pGPIO_IN_FLOATING:
        bank->set_mode(pin, pGPIO_MODE_IN);
        bank->set_pull(pin, pGPIO_PUPD_NONE);
        break;
      case pGPIO_IN_PULLUP:
        bank->set_mode(pin, pGPIO_MODE_IN);
        bank->set_pull(pin, pGPIO_PUPD_UP);
        break;
      case pGPIO_IN_PULLDOWN:
        bank->set_mode(pin, pGPIO_MODE_IN);
        bank->set_pull(pin, pGPIO_PUPD_DOWN);
        break;
      case pGPIO_IN_ANALOG:
        bank->set_mode(pin, pGPIO_MODE_AIN);
        break;
      case pGPIO_OUT_PP:
        bank->set_mode(pin, pGPIO_MODE_OUT);
        bank->set_type(pin, pGPIO_OTYPE_PP);
        bank->set_speed(pin, pGPIO_SPEED_LOW);
        bank->set_pull(pin, pGPIO_PUPD_NONE);
        break;
      case pGPIO_OUT_OD:
        bank->set_mode(pin, pGPIO_MODE_OUT);
        bank->set_type(pin, pGPIO_OTYPE_OD);
        bank->set_speed(pin, pGPIO_SPEED_LOW);
        bank->set_pull(pin, pGPIO_PUPD_NONE);
        break;
      case pGPIO_AF_PP:
        bank->set_mode(pin, pGPIO_MODE_ALT);
        bank->set_type(pin, pGPIO_OTYPE_PP);
        bank->set_speed(pin, pGPIO_SPEED_LOW);
        bank->set_pull(pin, pGPIO_PUPD_NONE);
        break;
      case pGPIO_AF_OD:
        bank->set_mode(pin, pGPIO_MODE_ALT);
        bank->set_type(pin, pGPIO_OTYPE_OD);
        bank->set_speed(pin, pGPIO_SPEED_LOW);
        bank->set_pull(pin, pGPIO_PUPD_NONE);
        break;
      case pGPIO_OUT_PP_PULLUP:
        bank->set_mode(pin, pGPIO_MODE_OUT);
        bank->set_type(pin, pGPIO_OTYPE_PP);
        bank->set_speed(pin, pGPIO_SPEED_LOW);
        bank->set_pull(pin, pGPIO_PUPD_UP);
        break;
      case pGPIO_OUT_OD_PULLUP:
        bank->set_mode(pin, pGPIO_MODE_OUT);
        bank->set_type(pin, pGPIO_OTYPE_OD);
        bank->set_speed(pin, pGPIO_SPEED_LOW);
        bank->set_pull(pin, pGPIO_PUPD_UP);
        break;
      case pGPIO_OUT_PP_PULLDOWN:
        bank->set_mode(pin, pGPIO_MODE_OUT);
        bank->set_type(pin, pGPIO_OTYPE_PP);
        bank->set_speed(pin, pGPIO_SPEED_LOW);
        bank->set_pull(pin, pGPIO_PUPD_DOWN);
        break;
      case pGPIO_OUT_OD_PULLDOWN:
        bank->set_mode(pin, pGPIO_MODE_OUT);
        bank->set_type(pin, pGPIO_OTYPE_OD);
        bank->set_speed(pin, pGPIO_SPEED_LOW);
        bank->set_pull(pin, pGPIO_PUPD_DOWN);
        break;
      case pGPIO_AF_PP_PULLUP:
        bank->set_mode(pin, pGPIO_MODE_ALT);
        bank->set_type(pin, pGPIO_OTYPE_PP);
        bank->set_speed(pin, pGPIO_SPEED_LOW);
        bank->set_pull(pin, pGPIO_PUPD_UP);
        break;
      case pGPIO_AF_OD_PULLUP:
        bank->set_mode(pin, pGPIO_MODE_ALT);
        bank->set_type(pin, pGPIO_OTYPE_OD);
        bank->set_speed(pin, pGPIO_SPEED_LOW);
        bank->set_pull(pin, pGPIO_PUPD_UP);
        break;
      case pGPIO_AF_PP_PULLDOWN:
        bank->set_mode(pin, pGPIO_MODE_ALT);
        bank->set_type(pin, pGPIO_OTYPE_PP);
        bank->set_speed(pin, pGPIO_SPEED_LOW);
        bank->set_pull(pin, pGPIO_PUPD_DOWN);
        break;
      case pGPIO_AF_OD_PULLDOWN:
        bank->set_mode(pin, pGPIO_MODE_ALT);
        bank->set_type(pin, pGPIO_OTYPE_OD);
        bank->set_speed(pin, pGPIO_SPEED_LOW);
        bank->set_pull(pin, pGPIO_PUPD_DOWN);
        break;
      default:
        status = pSTATUS_ERR;
        return;
    }

  // Mark the pin status as initialized.
  status = pSTATUS_SET;
}

/* Turn the GPIO pin on ('1') */
void bGPIO_PIN::on(void) {
  if (status == pSTATUS_ERR) { return; }
  bank->pin_on(pin);
}

/* Turn the GPIO pin off ('0') */
void bGPIO_PIN::off(void) {
  if (status == pSTATUS_ERR) { return; }
  bank->pin_off(pin);
}

/* Toggle the GPIO pin between on/off. */
void bGPIO_PIN::toggle(void) {
  if (status == pSTATUS_ERR) { return; }
  bank->pin_toggle(pin);
}

/* Read the pin's current state. */
bool bGPIO_PIN::read(void) {
  if (status == pSTATUS_ERR) { return 0; }
  return bank->read_pin(pin);
}

/* Getters/setters. */
// This checks if the pin is initialized; it
// does NOT read the value of an input pin.
int bGPIO_PIN::get_status(void) { return status; }


// Set the pin's MODER bits.
void bGPIO_PIN::set_mode(unsigned mode) {
  if (status == pSTATUS_ERR) { return; }
  bank->set_mode(pin, mode);
}

// Set the pin's OTYPER bit.
void bGPIO_PIN::set_type(unsigned type) {
  if (status == pSTATUS_ERR) { return; }
  bank->set_type(pin, type);
}

// Set the pin's OSPEEDR bits.
void bGPIO_PIN::set_speed(unsigned speed) {
  if (status == pSTATUS_ERR) { return; }
  bank->set_speed(pin, speed);
}

// Set the pin's PUPDR bits.
void bGPIO_PIN::set_pupd(unsigned pupd) {
  if (status == pSTATUS_ERR) { return; }
  bank->set_pull(pin, pupd);
}

// Set the pin's alternate function bits.
void bGPIO_PIN::set_alt_func(unsigned af) {
  if (status == pSTATUS_ERR) { return; }
  bank->set_af(pin, af);
}
