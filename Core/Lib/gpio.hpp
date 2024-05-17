/*
 * gpio.hpp
 *
 *  Created on: May 14, 2024
 *      Author: kevin.perillo
 */

#ifndef GPIO_H
#define GPIO_H

#include "core.h"
#include "stm32l4xx.h"

class GPIO : public CORE_IO {
public:
    enum Mode {
        INPUT = 0x00,
        OUTPUT = 0x01,
        ALTERNATE = 0x02,
        ANALOG = 0x03
    };

    enum Pull {
        NO_PULL = 0x00,
        PULL_UP = 0x01,
        PULL_DOWN = 0x02
    };

    enum Speed {
        LOW = 0x00,
        MEDIUM = 0x01,
        HIGH = 0x02,
        VERY_HIGH = 0x03
    };

    GPIO(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
    ~GPIO() {}

    void configure(Mode mode, Pull pull, Speed speed, uint8_t alternate = 0);

    void write(uint32_t data) override;
    unsigned read() const override;
    void stream(uint32_t* buffer) override;

    void toggle();
    void lock();
    void handle_exti_irq();
    void exti_callback();

private:
    GPIO_TypeDef* port;
    uint16_t pin;

    Mode mode;
    Pull pull;
    Speed speed;
    uint8_t alternate;

    void enable_gpio_clock();
};

#endif // GPIO_H
