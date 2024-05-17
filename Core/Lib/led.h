/*
 * led.h
 *
 *  Created on: May 16, 2024
 *      Author: kevin.perillo
 */

#ifndef LIB_LED_H_
#define LIB_LED_H_


#include "gpio.hpp"


class LED : public GPIO
{
	public:
		LED( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin );
		~LED( ) { }

		void configure_led( GPIO::Pull pull );

		uint8_t get_value( );
		void set_value( );
		void toggle( );

	protected:

	private:

	    GPIO_TypeDef* port;
	    uint16_t pin;
		uint8_t value;
};



#endif /* LIB_LED_H_ */
