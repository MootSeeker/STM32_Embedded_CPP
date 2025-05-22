/*
 * gpio.h
 *
 *  Created on: May 22, 2024
 *      Author: kevin.perillo
 */

#ifndef GPIO_H_
#define GPIO_H_

#ifdef UNIT_TESTING
    #include "mock_stm32l4xx.h" // For unit testing: uses mock peripherals
#else
    #include "stm32l4xx.h"      // For firmware: uses real STM32 headers
                                // Ensure CMake include paths are set up for "stm32l4xx.h"
                                // e.g., Drivers/CMSIS/Device/ST/STM32L4xx/Include
#endif

#include "common.h"

class GPIO {
public:
    enum class Port {
        PORTA, PORTB, PORTC, PORTD, PORTE, PORTF, PORTG, PORTH
    };

    enum class Mode {
        INPUT, OUTPUT, ALTERNATE, ANALOG
    };

    enum class Pull {
        NO_PULL, PULL_UP, PULL_DOWN
    };

    enum class Speed {
        LOW, MEDIUM, HIGH, VERY_HIGH
    };

    enum class AlternateFunction {
        AF0, AF1, AF2, AF3, AF4, AF5, AF6, AF7, AF8, AF9, AF10, AF11, AF12, AF13, AF14, AF15
    };

    GPIO(Port port, uint8_t pin);

    void setMode(Mode mode);
    void setPull(Pull pull);
    void setSpeed(Speed speed);
    void setAlternateFunction(AlternateFunction af);
    void write(bool value);
    bool read();

private:

    const uint8_t pin;
    GPIO_TypeDef* const portBase; // Pointer auf GPIO Register

    uint32_t getPinMask() const;
};


#endif /* GPIO_H_ */
