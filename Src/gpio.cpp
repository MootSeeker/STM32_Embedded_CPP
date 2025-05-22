/*
 * gpio.cpp
 *
 *  Created on: May 22, 2024
 *      Author: kevin.perillo
 */


#include "gpio.h"

// Constructor: Sets up GPIO port and enables its clock
GPIO::GPIO(Port port, uint8_t pin) : pin(pin), portBase([port]() -> GPIO_TypeDef* {
    GPIO_TypeDef* gpio = nullptr;
    uint32_t enableBit = 0;

    switch (port) {
        case Port::PORTA: gpio = GPIOA; enableBit = RCC_AHB2ENR_GPIOAEN; break;
        case Port::PORTB: gpio = GPIOB; enableBit = RCC_AHB2ENR_GPIOBEN; break;
        case Port::PORTC: gpio = GPIOC; enableBit = RCC_AHB2ENR_GPIOCEN; break;
#ifdef RCC_AHB2ENR_GPIODEN
        case Port::PORTD: gpio = GPIOD; enableBit = RCC_AHB2ENR_GPIODEN; break;
#endif
#ifdef RCC_AHB2ENR_GPIOEEN
        case Port::PORTE: gpio = GPIOE; enableBit = RCC_AHB2ENR_GPIOEEN; break;
#endif
#ifdef RCC_AHB2ENR_GPIOFEN
        case Port::PORTF: gpio = GPIOF; enableBit = RCC_AHB2ENR_GPIOFEN; break;
#endif
#ifdef RCC_AHB2ENR_GPIOGEN
        case Port::PORTG: gpio = GPIOG; enableBit = RCC_AHB2ENR_GPIOGEN; break;
#endif
        case Port::PORTH: gpio = GPIOH; enableBit = RCC_AHB2ENR_GPIOHEN; break;
        default: return nullptr;
    }

    if (gpio != nullptr) {
        RCC->AHB2ENR |= enableBit;
    }

    return gpio;
}()) {
    // Constructor body is empty - all initialization is done in member initializer list
}


// Diese Methode verwendet die Basisadresse, um den Pin-Modus zu setzen
void GPIO::setMode(Mode mode)
{
#ifdef UNIT_TESTING
    // Extra safety check for nullptr in test environment
    if (portBase == nullptr) {
        return; // Avoid segfault in tests
    }
#endif

    // Clear the mode bits for this pin (2 bits per pin)
    portBase->MODER &= ~(0x3UL << (pin * 2));

    // Set the new mode bits based on the requested mode
    switch (mode)
    {
        case Mode::INPUT:
            // Input mode is represented by 00, which is already set by the clear operation
            break;
        case Mode::OUTPUT:
            portBase->MODER |= (0x1UL << (pin * 2));
            break;
        case Mode::ALTERNATE:
            portBase->MODER |= (0x2UL << (pin * 2));
            break;
        case Mode::ANALOG:
        default:    // Default case is analog configuration to save power
            portBase->MODER |= (0x3UL << (pin * 2));
            break;
    }
}

void GPIO::setPull(Pull pull)
{
#ifdef UNIT_TESTING
    // Extra safety check for nullptr in test environment
    if (portBase == nullptr) {
        return; // Avoid segfault in tests
    }
#endif

    portBase->PUPDR &= ~(0x3UL << (pin * 2));
    switch (pull)
    {
        case Pull::PULL_UP:
            portBase->PUPDR |= (0x1UL << (pin * 2));
            break;
        case Pull::PULL_DOWN:
            portBase->PUPDR |= (0x2UL << (pin * 2));
            break;
        case Pull::NO_PULL:
        default:    // Default case should also be no pullup
            // No need to set anything, bits are already cleared
            break;
    }
}

void GPIO::setSpeed(Speed speed)
{
#ifdef UNIT_TESTING
    // Extra safety check for nullptr in test environment
    if (portBase == nullptr) {
        return; // Avoid segfault in tests
    }
#endif

    portBase->OSPEEDR &= ~(0x3UL << (pin * 2));
    switch (speed)
    {
        case Speed::MEDIUM:
            portBase->OSPEEDR |= (0x1UL << (pin * 2));
            break;
        case Speed::HIGH:
            portBase->OSPEEDR |= (0x2UL << (pin * 2));
            break;
        case Speed::VERY_HIGH:
            portBase->OSPEEDR |= (0x3UL << (pin * 2));
            break;
        case Speed::LOW:
        default:    // Default case is low
            // No need to set anything, bits are already cleared
            break;
    }
}

void GPIO::setAlternateFunction(AlternateFunction af)
{
    if(pin < 8)
    {
        portBase->AFR[0] &= ~(0xFUL << (pin * 4));
        portBase->AFR[0] |= (static_cast<uint32_t>(af) << (pin * 4));
    }
    else
    {
        portBase->AFR[1] &= ~(0xFUL << ((pin - 8) * 4));
        portBase->AFR[1] |= (static_cast<uint32_t>(af) << ((pin - 8) * 4));
    }
}

void GPIO::write(bool value)
{
    if(value)
    {
        portBase->BSRR = getPinMask();
    }
    else
    {
        portBase->BRR = getPinMask();
    }
}

bool GPIO::read()
{
    return (portBase->IDR & getPinMask()) != 0;
}

uint32_t GPIO::getPinMask() const
{
    return 1UL << pin;
}

