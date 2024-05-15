/*
 * gpio.h
 *
 *  Created on: May 12, 2024
 *      Author: kevin
 */

#ifndef LIB_PINS_H_
#define LIB_PINS_H_

#include "core.h"

#include <cstdint>

// Enums für PinMode, PinPull und PinSpeed
enum class PinMode {
    INPUT,
    OUTPUT,
    ANALOG,
    DAC_OUTPUT,
    OPEN_DRAIN_OUTPUT,
    // Weitere Modi können hier hinzugefügt werden
};

enum class PinPull {
    NONE,
    PULL_UP,
    PULL_DOWN
};

enum class PinSpeed {
    LOW,
    MEDIUM,
    HIGH,
    VERY_HIGH
};

class Pin {
public:
    GPIO_TypeDef* gpio; // Zeiger auf das GPIO-Register des Mikrocontrollers
    std::uint8_t pin; // Nummer des Pins auf dem GPIO-Port
    PinMode mode; // Aktueller Modus des Pins

    // Standardkonstruktor
    Pin() : gpio(nullptr), pin(0), mode(PinMode::INPUT) {}

    // Konstruktor, der den Pin initialisiert und den Modus festlegt
    Pin(GPIO_TypeDef* gpio, std::uint8_t pin, PinMode mode) : gpio(gpio), pin(pin), mode(mode) {
        configure(mode); // Konfigurieren des Pins im angegebenen Modus
    }

    // Methode zum Konfigurieren des Pins im angegebenen Modus
    void configure(PinMode mode) {
        gpio->MODER &= ~(0x3 << (pin * 2)); // Clear previous mode
        switch(mode) {
            case PinMode::OUTPUT:
                gpio->MODER |= (0x1 << (pin * 2));
                break;
            case PinMode::ANALOG:
                gpio->MODER |= (0x3 << (pin * 2));
                break;
            case PinMode::DAC_OUTPUT:
                gpio->MODER |= (0x2 << (pin * 2)); // Analog mode
                break;
            case PinMode::OPEN_DRAIN_OUTPUT:
                gpio->MODER |= (0x1 << (pin * 2)); // General purpose output mode
                gpio->OTYPER |= (0x1 << pin); // Set as open-drain
                break;
            default:
                break;
        }
    }

    // Methode zum Lesen des aktuellen Modus des Pins
    PinMode getMode() const {
        return mode;
    }

    // Methode zum Festlegen des Modus des Pins zur Laufzeit
    void setMode(PinMode newMode) {
        configure(newMode);
        mode = newMode;
    }

    // Methode zum Lesen und Setzen des Pull-Up/Pull-Down-Widerstands
    PinPull getPull() const {
        uint8_t pullSetting = (gpio->PUPDR >> (pin * 2)) & 0x3;
        switch (pullSetting) {
            case 0x1:
                return PinPull::PULL_UP;
            case 0x2:
                return PinPull::PULL_DOWN;
            default:
                return PinPull::NONE;
        }
    }

    void setPull(PinPull pull) {
        if (mode == PinMode::INPUT) {
            gpio->PUPDR &= ~(0x3 << (pin * 2));
            switch (pull) {
                case PinPull::PULL_UP:
                    gpio->PUPDR |= (0x1 << (pin * 2));
                    break;
                case PinPull::PULL_DOWN:
                    gpio->PUPDR |= (0x2 << (pin * 2));
                    break;
                case PinPull::NONE:
                    // No pull-up/pull-down
                    break;
            }
        }
    }

    // Methode zum Lesen und Setzen der Ausgangsgeschwindigkeit des Pins
    void setOutputSpeed(PinSpeed speed) {
        if (mode == PinMode::OUTPUT) {
            gpio->OSPEEDR &= ~(0x3 << (pin * 2));
            switch (speed) {
                case PinSpeed::LOW:
                    // Low speed output
                    break;
                case PinSpeed::MEDIUM:
                    gpio->OSPEEDR |= (0x1 << (pin * 2));
                    break;
                case PinSpeed::HIGH:
                    gpio->OSPEEDR |= (0x2 << (pin * 2));
                    break;
                case PinSpeed::VERY_HIGH:
                    gpio->OSPEEDR |= (0x3 << (pin * 2));
                    break;
            }
        }
    }

    // Methode zum Lesen und Schreiben des Schreibschutzbits (LCKR)
    bool isLocked() const {
        return (gpio->LCKR & (0x1 << pin)) != 0;
    }

    // Methode zum Setzen des Schreibschutzbits (Sperren des Pins)
    void lock() {
        gpio->LCKR = (0x1 << pin); // Setzen des Schreibschutzbits
        gpio->LCKR = (0x1 << pin); // Erneutes Schreiben zum Verriegeln
    }

    // Methode zum Löschen des Schreibschutzbits (Entsperren des Pins)
    void unlock() {
        gpio->LCKR = (0x1 << 16) | pin; // Löschen des Schreibschutzbits
    }

    // Methode zum Schreiben des Zustands des Pins
    void write(bool state) {
        if (mode == PinMode::OUTPUT) {
            if (state) {
                gpio->BSRR = (1 << pin);
            } else {
                gpio->BSRR = (1 << (pin + 16));
            }
        }
    }

    // Methode zum Umkehren des Zustands des Pins
    void toggle() {
        if (mode == PinMode::OUTPUT) {
            gpio->ODR ^= (1 << pin);
        }
    }

    // Methoden zum Lesen und Schreiben des Zustands des Pins
    bool read() const {
        if (mode == PinMode::INPUT) {
            return (gpio->IDR & (0x1 << pin)) != 0;
        }
        return false;
    }
};


#endif /* LIB_PINS_H_ */
