/*
 * gpio.h
 *
 *  Created on: May 12, 2024
 *      Author: kevin
 */

#ifndef LIB_PINS_H_
#define LIB_PINS_H_

#include "base.h"

// Project macro definitions.
// GPIO pin state macros.

  #define pGPIO_MODE_IN        (0x00)
  #define pGPIO_MODE_OUT       (0x01)
  #define pGPIO_MODE_ALT       (0x02)
  #define pGPIO_MODE_AIN       (0x03)
  #define pGPIO_OTYPE_PP       (0x00)
  #define pGPIO_OTYPE_OD       (0x01)
  #define pGPIO_SPEED_LOW      (0x00)
  #define pGPIO_SPEED_MED      (0x01)
  #define pGPIO_SPEED_HI       (0x03)
  #define pGPIO_PUPD_NONE      (0x00)
  #define pGPIO_PUPD_UP        (0x01)
  #define pGPIO_PUPD_DOWN      (0x02)

// GPIO enum for convenience instantiation.
// Output speed is not currently specified; it can
// be set after the initialization, but 2MHz /
// 'low speed' is fine for most purposes.
enum pGPIO_pin_qinit {
  pGPIO_IN_FLOATING,
  pGPIO_IN_PULLUP,
  pGPIO_IN_PULLDOWN,
  pGPIO_IN_ANALOG,
  pGPIO_OUT_PP,
  pGPIO_OUT_OD,
  pGPIO_AF_PP,
  pGPIO_AF_OD,
  pGPIO_OUT_PP_PULLUP,
  pGPIO_OUT_PP_PULLDOWN,
  pGPIO_OUT_OD_PULLUP,
  pGPIO_OUT_OD_PULLDOWN,
  pGPIO_AF_PP_PULLUP,
  pGPIO_AF_PP_PULLDOWN,
  pGPIO_AF_OD_PULLUP,
  pGPIO_AF_OD_PULLDOWN,
};


class bGPIO : public bIO
{
public:

	bGPIO( );
	bGPIO( GPIO_TypeDef* port );

	// Common read / write methods from Base class
	unsigned read( void );
	void write( unsigned data );
    void stream( volatile void *buffer, int length );

    // Generic GPIO methods
    int read_pin(unsigned pin_num);
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


class bGPIO_PIN
{
public:
	bGPIO_PIN( );
	  // Convenience constructor.
	  // Pass in an enum value from the pin modes defined above.
	bGPIO_PIN(bGPIO* pin_bank, uint8_t pin_num, pGPIO_pin_qinit q);
	  // GPIO pin methods.
	  // TODO: add a flag for reversing 'on/off' for e.g. a pin
	  // wired to an LED's cathode.
	  void on(void);
	  void off(void);
	  void toggle(void);
	  bool read(void);
	  // Getters/setters.
	  int  get_status(void);
	  // Platform-specific pin configuration methods.
	  void set_mode(unsigned mode);
	  void set_type(unsigned type);
	  void set_speed(unsigned speed);
	  void set_pupd(unsigned pupd);
	  void set_alt_func(unsigned af);

protected:
	  bGPIO*  bank   = NULL;
	  uint8_t pin    = 0;
	  // Expected pin status.
	  int status     = pSTATUS_ERR;
private:

};





#endif /* LIB_PINS_H_ */
