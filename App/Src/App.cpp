/**
 * Filename: App.cpp
 * @brief Application example using GPIO library with LED and buttons
 * @author MootSeeker
 * @date Sep 17, 2025
 * 
 * This example demonstrates:
 * - LED control on PB11
 * - Button interrupts on PC0, PC1, PC2, PC3
 * - GPIO library usage with interrupts
 */

#include "App.h"
#include "main.h"

#include "gpio.h"

#include <cstdio>

using namespace GPIO;

// Global GPIO objects
static GPIOOutput* led = nullptr;
static GPIOEXTI* btn0 = nullptr;
static GPIOEXTI* btn1 = nullptr; 
static GPIOEXTI* btn2 = nullptr;
static GPIOEXTI* btn3 = nullptr;

// LED patterns
static uint32_t ledPattern = 0;

/**
 * @brief C wrapper for GPIO interrupt handling
 * 
 * This function is called from the C interrupt handlers in stm32l4xx_it.c
 * and forwards the call to the C++ GPIO interrupt system.
 */
extern "C" void GPIO_EXTI_HandleInterrupt(uint32_t pin)
{
    GPIO::GPIOEXTI::handleInterrupt(pin);
}

/**
 * @brief Button 0 (PC0) interrupt callback
 * 
 * Toggles the LED when button 0 is pressed
 */
void btn0InterruptCallback()
{
    printf("Button 0 pressed - Toggling LED\n");
    if (led) {
        led->toggle();
    }
}

/**
 * @brief Button 1 (PC1) interrupt callback
 * 
 * Turns LED on when button 1 is pressed
 */
void btn1InterruptCallback()
{
    printf("Button 1 pressed - LED ON\n");
    if (led) {
        led->set();
    }
}

/**
 * @brief Button 2 (PC2) interrupt callback
 * 
 * Turns LED off when button 2 is pressed  
 */
void btn2InterruptCallback()
{
    printf("Button 2 pressed - LED OFF\n");
    if (led) {
        led->reset();
    }
}

/**
 * @brief Button 3 (PC3) interrupt callback
 * 
 * Cycles through LED patterns when button 3 is pressed
 */
void btn3InterruptCallback()
{
    ledPattern = (ledPattern + 1) % 4;
    printf("Button 3 pressed - LED Pattern: %lu\n", ledPattern);
    
    if (led) {
        switch (ledPattern) {
            case 0:
                led->reset();
                printf("Pattern: OFF\n");
                break;
            case 1:
                led->set();
                printf("Pattern: ON\n");
                break;
            case 2:
                // Rapid toggle pattern will be handled in main loop
                printf("Pattern: SLOW BLINK\n");
                break;
            case 3:
                // Fast toggle pattern will be handled in main loop
                printf("Pattern: FAST BLINK\n");
                break;
        }
    }
}

void App_Init(void)
{
    printf("App_Init: Initializing GPIO example...\n");
    
    // Create LED on PB11 (push-pull output, low speed)
    led = new GPIOOutput(GPIOB, 11, PinOutputType::PUSH_PULL, PinSpeed::LOW);
    
    // Create buttons on PC0-PC3 with pull-up resistors and falling edge trigger
    btn0 = new GPIOEXTI(GPIOC, 0, EXTITrigger::FALLING, PinPull::PULL_UP);
    btn1 = new GPIOEXTI(GPIOC, 1, EXTITrigger::FALLING, PinPull::PULL_UP);
    btn2 = new GPIOEXTI(GPIOC, 2, EXTITrigger::FALLING, PinPull::PULL_UP);
    btn3 = new GPIOEXTI(GPIOC, 3, EXTITrigger::FALLING, PinPull::PULL_UP);
    
    // Set up interrupt callbacks
    btn0->setCallback(btn0InterruptCallback);
    btn1->setCallback(btn1InterruptCallback);
    btn2->setCallback(btn2InterruptCallback);  
    btn3->setCallback(btn3InterruptCallback);
    
    // Enable interrupts
    btn0->enableInterrupt();
    btn1->enableInterrupt();
    btn2->enableInterrupt();
    btn3->enableInterrupt();
    
    // Start with LED off
    led->reset();
    ledPattern = 0;
    
    printf("GPIO Example initialized:\n");
    printf("- LED on PB11\n");
    printf("- Button 0 (PC0): Toggle LED\n");
    printf("- Button 1 (PC1): LED ON\n");
    printf("- Button 2 (PC2): LED OFF\n");
    printf("- Button 3 (PC3): Cycle LED patterns\n");
}

void App_Run(void)
{
    printf("App_Run: Starting main application loop\n");
    
    uint32_t slowBlinkCounter = 0;
    uint32_t fastBlinkCounter = 0;
    
    while (true) {
        // Handle LED blinking patterns
        switch (ledPattern) {
            case 2: // Slow blink
                slowBlinkCounter++;
                if (slowBlinkCounter >= 500000) { // Adjust timing as needed
                    led->toggle();
                    slowBlinkCounter = 0;
                }
                break;
                
            case 3: // Fast blink
                fastBlinkCounter++;
                if (fastBlinkCounter >= 100000) { // Adjust timing as needed
                    led->toggle();
                    fastBlinkCounter = 0;
                }
                break;
                
            default:
                // Reset counters for patterns 0 and 1
                slowBlinkCounter = 0;
                fastBlinkCounter = 0;
                break;
        }
        
        // Small delay to prevent busy waiting
        for (volatile int i = 0; i < 100; i++);
    }
}
