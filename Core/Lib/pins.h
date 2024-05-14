/*
 * gpio.h
 *
 *  Created on: May 12, 2024
 *      Author: kevin
 */

#ifndef LIB_PINS_H_
#define LIB_PINS_H_

#include "core.h"

class Pin {
public:
    volatile uint32_t* port;
    uint8_t pin;

    Pin(volatile uint32_t* port, uint8_t pin) : port(port), pin(pin) {
        // Konfigurieren Sie den Pin als Ausgang
        *port &= ~(0x3 << (pin * 2));
        *port |= (0x1 << (pin * 2));
    }

    void toggle() {
        *(port+5) ^= (0x1 << pin); // Zugriff auf ODR-Register zum Umschalten des Pins
    }
};

#endif /* LIB_PINS_H_ */
