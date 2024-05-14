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

class GPIO {
public:
    Pin* PB3;

    GPIO() {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;  // Takt für GPIOB aktivieren
        PB3 = new Pin(&GPIOB->MODER, 3);
    }

    ~GPIO() {
        delete PB3;
    }
};


#endif /* LIB_GPIO_HPP_ */
