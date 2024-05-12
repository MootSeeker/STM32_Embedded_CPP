/*
 * gpio.h
 *
 *  Created on: May 12, 2024
 *      Author: kevin
 */

#ifndef LIB_PINS_H_
#define LIB_PINS_H_

#include "base.h"


class bGPIO : public bIO
{
public:

	bGPIO( );
	bGPIO( GPIO_TypeDef* port );

	// Common read / write methods from Base class
	unsigned read( void );
	void write( unsigned data );
    /**
     * @brief Stream data.
     * @param buffer Pointer to the data buffer.
     * @param length Length of the buffer.
     */
    void stream( volatile void *buffer, int length );

    // Generic GPIO methods
    GPIO_PinState read_pin(unsigned pin_num);
    void     pin_on(unsigned pin_num);
    void     pin_off(unsigned pin_num);
    void     pin_toggle(unsigned pin_num);

	// GPIO class specific methods
	void set_type( unsigned pin, unsigned type );						/*!< Specifies the GPIO pins to be configured.
                           	   	   	   	   	   	   	   	   This parameter can be any value of @ref GPIO_pins */
	void set_mode( unsigned pin, unsigned mode );		/*!< Specifies the operating mode for the selected pins.
                           	   	   	   	   	   	   	   	   This parameter can be a value of @ref GPIO_mode */
	void set_pull( unsigned pin, unsigned pull_type );	/*!< Specifies the Pull-up or Pull-Down activation for the selected pins.
                           	   	   	   	   	   	   	   	   This parameter can be a value of @ref GPIO_pull */
	void set_speed( unsigned pin, unsigned speed );		/*!< Specifies the speed for the selected pins.
                           	   	   	   	   	   	   	   	   This parameter can be a value of @ref GPIO_speed */
	void set_af( unsigned pin, unsigned af ); 			/*!< Peripheral to be connected to the selected pins
                            								This parameter can be a value of @ref GPIOEx_Alternate_function_selection */
protected:

	// Reference to GPIO register struct
	GPIO_TypeDef* gpio = NULL;

private:

};





#endif /* LIB_PINS_H_ */
