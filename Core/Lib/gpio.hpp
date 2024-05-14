/*
 * gpio.hpp
 *
 *  Created on: May 14, 2024
 *      Author: kevin.perillo
 */

#ifndef LIB_GPIO_HPP_
#define LIB_GPIO_HPP_

#include "core.h"
#include "pins.h"

#include <memory>
#include <array>

class GPIO {
public:
    std::array<std::unique_ptr<Pin>, 16> pins;
    GPIO_TypeDef* gpio;

    GPIO(GPIO_TypeDef* gpio) : gpio(gpio) {
        if(gpio == GPIOA) {
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
        } else if(gpio == GPIOB) {
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
        }
        // Fügen Sie hier weitere else if Anweisungen für andere GPIOs hinzu

        for(int i=0; i<16; i++) {
            pins[i] = std::make_unique<Pin>(gpio, i, PinMode::OUTPUT);
        }
    }
};


#endif /* LIB_GPIO_HPP_ */
