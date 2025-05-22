#include "catch.hpp"
#include "gpio.h"           // The GPIO class header being tested
#include "mock_stm32l4xx.h" // For direct access to mock registers and reset_all_mocks

// This ensures that when gpio.h is included, it uses the mock peripherals
// because UNIT_TESTING will be defined by the CMakeLists.txt for test builds.

TEST_CASE("GPIO Constructor", "[gpio]") {
    SECTION("Constructor enables clock and sets up port") {
        reset_all_mocks();
        
        // Pre-condition: RCC AHB2ENR starts at 0
        REQUIRE(mock_RCC_Peripheral.AHB2ENR == 0);

        // Create a GPIO object for PORTA pin 0
        GPIO pa0(GPIO::Port::PORTA, 0);
        
        // Check that clock was enabled for GPIOA
        REQUIRE((mock_RCC_Peripheral.AHB2ENR & RCC_AHB2ENR_GPIOAEN) == RCC_AHB2ENR_GPIOAEN);
    }
}

TEST_CASE("GPIO Mode Setting", "[gpio]") {
    SECTION("Output Mode") {
        reset_all_mocks();
        
        // Create a GPIO for Port A, pin 5
        GPIO pa5(GPIO::Port::PORTA, 5);
        
        // Verify initial state
        REQUIRE(mock_GPIOA_Peripheral.MODER == 0);
        
        // Set mode to output
        pa5.setMode(GPIO::Mode::OUTPUT);
        
        // Check that mode bits for pin 5 (bits 10-11) are set to output (01)
        uint32_t modeBits = (mock_GPIOA_Peripheral.MODER & (0x3UL << (5 * 2))) >> (5 * 2);
        REQUIRE(modeBits == 1); // 1 for output mode
    }

    SECTION("Input Mode") {
        reset_all_mocks();
        
        GPIO pa5(GPIO::Port::PORTA, 5);
        pa5.setMode(GPIO::Mode::INPUT);
        
        // Check that mode bits are cleared (00 for input)
        uint32_t modeBits = (mock_GPIOA_Peripheral.MODER & (0x3UL << (5 * 2))) >> (5 * 2);
        REQUIRE(modeBits == 0);
    }

    SECTION("Alternate Function Mode") {
        reset_all_mocks();
        
        GPIO pa5(GPIO::Port::PORTA, 5);
        pa5.setMode(GPIO::Mode::ALTERNATE);
        
        // Check that mode bits are set to 10 for alternate function
        uint32_t modeBits = (mock_GPIOA_Peripheral.MODER & (0x3UL << (5 * 2))) >> (5 * 2);
        REQUIRE(modeBits == 2);
    }

    SECTION("Analog Mode") {
        reset_all_mocks();
        
        GPIO pa5(GPIO::Port::PORTA, 5);
        pa5.setMode(GPIO::Mode::ANALOG);
        
        // Check that mode bits are set to 11 for analog mode
        uint32_t modeBits = (mock_GPIOA_Peripheral.MODER & (0x3UL << (5 * 2))) >> (5 * 2);
        REQUIRE(modeBits == 3);
    }
}

TEST_CASE("GPIO Pull-up/Pull-down Setting", "[gpio]") {
    SECTION("No Pull") {
        reset_all_mocks();
        
        GPIO pa5(GPIO::Port::PORTA, 5);
        pa5.setPull(GPIO::Pull::NO_PULL);
        
        uint32_t pullBits = (mock_GPIOA_Peripheral.PUPDR & (0x3UL << (5 * 2))) >> (5 * 2);
        REQUIRE(pullBits == 0);
    }

    SECTION("Pull-up") {
        reset_all_mocks();
        
        GPIO pa5(GPIO::Port::PORTA, 5);
        pa5.setPull(GPIO::Pull::PULL_UP);
        
        uint32_t pullBits = (mock_GPIOA_Peripheral.PUPDR & (0x3UL << (5 * 2))) >> (5 * 2);
        REQUIRE(pullBits == 1);
    }

    SECTION("Pull-down") {
        reset_all_mocks();
        
        GPIO pa5(GPIO::Port::PORTA, 5);
        pa5.setPull(GPIO::Pull::PULL_DOWN);
        
        uint32_t pullBits = (mock_GPIOA_Peripheral.PUPDR & (0x3UL << (5 * 2))) >> (5 * 2);
        REQUIRE(pullBits == 2);
    }
}