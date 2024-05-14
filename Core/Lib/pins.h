/*
 * gpio.h
 *
 *  Created on: May 12, 2024
 *      Author: kevin
 */

#ifndef LIB_PINS_H_
#define LIB_PINS_H_

#include "core.h"


enum class PinMode {
    INPUT,
    OUTPUT,
    ANALOG,
    // Fügen Sie hier weitere Modi hinzu
};

class Pin {
public:
    GPIO_TypeDef* gpio;
    uint8_t pin;
    PinMode mode;

    Pin(GPIO_TypeDef* gpio, uint8_t pin, PinMode mode) : gpio(gpio), pin(pin), mode(mode) {
        configure(mode);
    }

    void configure(PinMode mode) {
        switch(mode) {
            case PinMode::OUTPUT:
                gpio->MODER &= ~(0x3 << (pin * 2));
                gpio->MODER |= (0x1 << (pin * 2));
                break;
            case PinMode::INPUT:
                gpio->MODER &= ~(0x3 << (pin * 2));
                break;
            case PinMode::ANALOG:
                gpio->MODER |= (0x3 << (pin * 2));
                break;
            // Fügen Sie hier weitere Fälle für andere Modi hinzu
        }
    }

    void toggle() {
        gpio->ODR ^= (0x1 << pin);
    }

    void set() {
        gpio->ODR |= (0x1 << pin);
    }

    void reset() {
        gpio->ODR &= ~(0x1 << pin);
    }
};

#endif /* LIB_PINS_H_ */
