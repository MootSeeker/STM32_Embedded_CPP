/*
 * button.h
 *
 *  Created on: May 16, 2024
 *      Author: kevin.perillo
 */

#ifndef LIB_BUTTON_H_
#define LIB_BUTTON_H_


#include "gpio.hpp"

class Button: public GPIO
{
	public:
		Button( GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin );
		void configure_button( GPIO::Pull pull );

	private:
		void configure_exti( );
};


#endif /* LIB_BUTTON_H_ */
