/**
 * @file gpio.h
 * @brief Intelligent GPIO class library for STM32L4xx using LL drivers
 * @author kevin
 * @date Sep 16, 2025
 * @version 1.0
 * 
 * This file provides a comprehensive C++ wrapper for STM32L4xx GPIO operations
 * using the Low-Level (LL) driver library. It implements an object-oriented
 * approach with inheritance hierarchy for different GPIO functionalities.
 * 
 * @note This library requires STM32L4xx LL drivers to be included in the project
 * @warning Ensure proper clock configuration before using any GPIO functionality
 */

#ifndef DEVICE_INC_GPIO_H_
#define DEVICE_INC_GPIO_H_

#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_exti.h"

#include <vector>
#include <cstdint>
#include <functional>

/**
 * @namespace GPIO
 * @brief Contains all GPIO-related classes, enumerations, and utility functions
 * 
 * This namespace encapsulates the entire GPIO functionality providing a clean
 * interface for GPIO operations on STM32L4xx microcontrollers.
 */
namespace GPIO
{
    /**
     * @enum PinMode
     * @brief Enumeration for GPIO pin modes
     * 
     * Defines the different operational modes available for GPIO pins.
     * Each mode corresponds to a specific hardware configuration.
     */
    enum class PinMode : uint32_t
    {
        INPUT = LL_GPIO_MODE_INPUT,          ///< Input mode - pin reads digital signals
        OUTPUT = LL_GPIO_MODE_OUTPUT,        ///< Output mode - pin drives digital signals
        ALTERNATE = LL_GPIO_MODE_ALTERNATE,  ///< Alternate function mode - pin used by peripherals
        ANALOG = LL_GPIO_MODE_ANALOG         ///< Analog mode - pin used for analog signals
    }; 

    /**
     * @enum PinPull
     * @brief Enumeration for GPIO pin pull-up/pull-down configurations
     * 
     * Defines the internal pull resistor configuration for input pins.
     * These resistors help define logic levels when no external signal is present.
     */
    enum class PinPull : uint32_t
    {
        NO_PULL = LL_GPIO_PULL_NO,       ///< No internal pull resistor - floating input
        PULL_UP = LL_GPIO_PULL_UP,       ///< Internal pull-up resistor enabled - defaults to HIGH
        PULL_DOWN = LL_GPIO_PULL_DOWN    ///< Internal pull-down resistor enabled - defaults to LOW
    }; 

    /**
     * @enum PinSpeed
     * @brief Enumeration for GPIO pin speed configurations
     * 
     * Defines the slew rate and drive strength for output pins.
     * Higher speeds consume more power but provide faster transitions.
     */
    enum class PinSpeed : uint32_t
    {
        LOW = LL_GPIO_SPEED_FREQ_LOW,           ///< Low speed - up to 8 MHz, lowest power consumption
        MEDIUM = LL_GPIO_SPEED_FREQ_MEDIUM,     ///< Medium speed - up to 25 MHz, balanced performance
        HIGH = LL_GPIO_SPEED_FREQ_HIGH,         ///< High speed - up to 50 MHz, higher power consumption
        VERY_HIGH = LL_GPIO_SPEED_FREQ_VERY_HIGH ///< Very high speed - up to 80 MHz, highest power consumption
    };

    /**
     * @enum PinOutputType
     * @brief Enumeration for GPIO pin output types
     * 
     * Defines the electrical characteristics of output pins.
     * Affects how the pin drives logic levels.
     */
    enum class PinOutputType : uint32_t
    {
        PUSH_PULL = LL_GPIO_OUTPUT_PUSHPULL,   ///< Push-pull output - actively drives both HIGH and LOW
        OPEN_DRAIN = LL_GPIO_OUTPUT_OPENDRAIN  ///< Open-drain output - only pulls LOW, external pullup needed for HIGH
    };

    /**
     * @enum PinState
     * @brief Enumeration for GPIO pin logical states
     * 
     * Represents the logical state of a GPIO pin.
     * Maps to actual voltage levels on the pin.
     */
    enum class PinState : bool
    {
        LOW = 0,   ///< Logic low state (0V)
        HIGH = 1   ///< Logic high state (VDD, typically 3.3V)
    };

    /**
     * @enum EXTITrigger
     * @brief Enumeration for EXTI trigger types
     * 
     * Defines when an external interrupt should be triggered.
     * Determines the edge sensitivity of interrupt pins.
     */
    enum class EXTITrigger : uint32_t
    {
        RISING = LL_EXTI_TRIGGER_RISING,           ///< Trigger on rising edge (LOW to HIGH transition)
        FALLING = LL_EXTI_TRIGGER_FALLING,         ///< Trigger on falling edge (HIGH to LOW transition)
        RISING_FALLING = LL_EXTI_TRIGGER_RISING_FALLING ///< Trigger on both edges (any transition)
    };

    // Forward declarations
    class GPIOBase;
    class GPIOOutput;
    class GPIOInput;
    class GPIOEXTI;

    /**
     * @typedef InterruptCallback
     * @brief Function pointer type for interrupt callbacks
     * 
     * Defines the signature for interrupt callback functions.
     * These functions are called from interrupt context, so they should be fast and non-blocking.
     */
    using InterruptCallback = std::function<void(void)>;

    /**
     * @struct PinConfig
     * @brief Configuration structure for GPIO pins
     * 
     * Contains all parameters needed to configure a GPIO pin.
     * Provides default values for common use cases.
     */
    struct PinConfig
    {
        GPIO_TypeDef* port;                              ///< GPIO port (GPIOA, GPIOB, etc.)
        uint32_t pin;                                    ///< Pin number (0-15)
        PinMode mode = PinMode::INPUT;                   ///< Operating mode
        PinPull pull = PinPull::NO_PULL;                 ///< Pull resistor configuration
        PinSpeed speed = PinSpeed::LOW;                  ///< Output speed (for output pins)
        PinOutputType outputType = PinOutputType::PUSH_PULL; ///< Output driver type
        uint32_t alternate = 0;                          ///< Alternate function number (0-15)
        EXTITrigger trigger = EXTITrigger::RISING;       ///< Interrupt trigger type (for EXTI pins)
    }; 

    /**
     * @class GPIOBase
     * @brief Base class for all GPIO operations providing common functionality
     * 
     * This abstract base class provides the common functionality shared by all GPIO classes.
     * It handles hardware initialization, clock management, and basic configuration.
     * Derived classes implement specific functionality for their respective use cases.
     * 
     * @note This class should not be instantiated directly. Use derived classes instead.
     * @see GPIOOutput, GPIOInput, GPIOEXTI
     */
    class GPIOBase
    {
    protected:
        GPIO_TypeDef* port_;        ///< GPIO port register base address
        uint32_t pin_;              ///< GPIO pin number (0-15)
        PinConfig config_;          ///< Complete pin configuration structure

        /**
         * @brief Enable the clock for the specified GPIO port
         * 
         * Automatically enables the appropriate AHB2 clock for the GPIO port.
         * This is required before any GPIO operations can be performed.
         * 
         * @param port GPIO port to enable clock for (GPIOA, GPIOB, etc.)
         * @note Clock is enabled only once per port, multiple calls are safe
         */
        void enablePortClock();

        /**
         * @brief Convert pin number to LL GPIO pin mask
         * 
         * Converts a pin number (0-15) to the corresponding LL library pin mask.
         * This is used internally for LL driver function calls.
         * 
         * @param pin Pin number (0-15)
         * @return LL GPIO pin mask (1 << pin)
         * @warning Pin number must be in range 0-15
         */
        uint32_t getLLPin(uint32_t pin) const;

        /**
         * @brief Configure the GPIO pin with current configuration
         * 
         * Applies the current configuration stored in config_ to the hardware.
         * This method is called automatically during initialization and
         * when configuration parameters are changed.
         * 
         * @note Assumes port clock is already enabled
         */
        void configureHardware();

    public:
        /**
         * @brief Construct a new GPIOBase object with basic configuration
         * 
         * Creates a GPIO object with the specified port, pin, and mode.
         * Other parameters use default values and can be configured later.
         * 
         * @param port GPIO port (GPIOA, GPIOB, GPIOC, etc.)
         * @param pin GPIO pin number (0-15)
         * @param mode Initial pin mode (INPUT, OUTPUT, ALTERNATE, ANALOG)
         * 
         * @warning Pin number must be in range 0-15
         * @note Port clock is automatically enabled
         */
        GPIOBase(GPIO_TypeDef* port, uint32_t pin, PinMode mode = PinMode::INPUT);

        /**
         * @brief Construct a new GPIOBase object with full configuration
         * 
         * Creates a GPIO object using a complete configuration structure.
         * This allows setting all parameters at once during construction.
         * 
         * @param config Complete pin configuration structure
         * 
         * @note Port clock is automatically enabled
         * @see PinConfig
         */
        GPIOBase(const PinConfig& config);

        /**
         * @brief Virtual destructor for proper inheritance
         * 
         * Ensures proper cleanup when derived objects are deleted through base pointer.
         * Default implementation is sufficient for this class.
         */
        virtual ~GPIOBase() = default;

        // Configuration methods
        /**
         * @brief Set the operating mode of the GPIO pin
         * @param mode New pin mode (INPUT, OUTPUT, ALTERNATE, ANALOG)
         * @note Changes are applied immediately to hardware
         */
        void setMode(PinMode mode);
        
        /**
         * @brief Set the pull resistor configuration
         * @param pull Pull resistor setting (NO_PULL, PULL_UP, PULL_DOWN)
         * @note Only affects input pins, changes applied immediately
         */
        void setPull(PinPull pull);
        
        /**
         * @brief Set the output speed/drive strength
         * @param speed Output speed setting (LOW, MEDIUM, HIGH, VERY_HIGH)
         * @note Only affects output pins, changes applied immediately
         */
        void setSpeed(PinSpeed speed);
        
        /**
         * @brief Set the output driver type
         * @param outputType Driver type (PUSH_PULL, OPEN_DRAIN)
         * @note Only affects output pins, changes applied immediately
         */
        void setOutputType(PinOutputType outputType);
        
        /**
         * @brief Set the alternate function number
         * @param alternate Alternate function number (0-15)
         * @note Only applies when mode is ALTERNATE, changes applied immediately
         */
        void setAlternate(uint32_t alternate);

        // Getters
        /**
         * @brief Get the GPIO port
         * @return GPIO port register base address
         */
        GPIO_TypeDef* getPort() const { return port_; }
        
        /**
         * @brief Get the pin number
         * @return Pin number (0-15)
         */
        uint32_t getPin() const { return pin_; }
        
        /**
         * @brief Get the current pin mode
         * @return Current operating mode
         */
        PinMode getMode() const { return config_.mode; }
        
        /**
         * @brief Get the complete pin configuration
         * @return Copy of the current configuration structure
         */
        PinConfig getConfig() const { return config_; }

        // Pure virtual methods to be implemented by derived classes
        /**
         * @brief Validate if current configuration is valid for this pin type
         * @return true if configuration is valid, false otherwise
         * 
         * Pure virtual method that allows derived classes to validate
         * their specific configuration requirements.
         */
        virtual bool isValidForMode() const = 0;
    };

    /**
     * @class GPIOOutput
     * @brief GPIO Output pin class with output-specific functionality
     * 
     * Specialized GPIO class for output operations. Provides methods for
     * writing digital values, setting/resetting pins, and toggling states.
     * Tracks the current output state for optimization and debugging.
     * 
     * @note Pin is automatically configured as OUTPUT mode
     * @see GPIOBase
     */
    class GPIOOutput : public GPIOBase
    {
    private:
        PinState currentState_;     ///< Current output state for tracking

    public:
        /**
         * @brief Construct a new GPIOOutput object with basic configuration
         * 
         * Creates an output pin with specified electrical characteristics.
         * The pin is automatically configured as OUTPUT mode and initialized to LOW state.
         * 
         * @param port GPIO port (GPIOA, GPIOB, etc.)
         * @param pin GPIO pin number (0-15)
         * @param outputType Output driver type (PUSH_PULL or OPEN_DRAIN)
         * @param speed Output speed/drive strength
         * 
         * @warning Pin number must be in range 0-15
         * @note Pin is automatically initialized to LOW state
         */
        GPIOOutput(GPIO_TypeDef* port, uint32_t pin, 
                   PinOutputType outputType = PinOutputType::PUSH_PULL,
                   PinSpeed speed = PinSpeed::LOW);

        /**
         * @brief Construct a new GPIOOutput object with full configuration
         * 
         * Creates an output pin using a complete configuration structure.
         * Mode is automatically set to OUTPUT regardless of config.mode value.
         * 
         * @param config Complete pin configuration structure
         * @note config.mode is forced to OUTPUT
         */
        GPIOOutput(const PinConfig& config);

        // Override base class methods
        /**
         * @brief Validate configuration for output mode
         * @return true if pin is configured as OUTPUT
         */
        bool isValidForMode() const override;

        // Output-specific methods
        /**
         * @brief Write a digital value to the output pin
         * 
         * Sets the pin to the specified logic level and updates internal state tracking.
         * 
         * @param state Logic level to write (HIGH or LOW)
         * @note State change is immediate
         */
        void write(PinState state);
        
        /**
         * @brief Set the output pin to HIGH
         * 
         * Convenience method equivalent to write(PinState::HIGH).
         * Updates internal state tracking.
         */
        void set();
        
        /**
         * @brief Set the output pin to LOW
         * 
         * Convenience method equivalent to write(PinState::LOW).
         * Updates internal state tracking.
         */
        void reset();
        
        /**
         * @brief Toggle the current output state
         * 
         * Changes HIGH to LOW or LOW to HIGH.
         * Updates internal state tracking.
         */
        void toggle();
        
        /**
         * @brief Get the current output state
         * @return Current logic level (HIGH or LOW)
         * @note Returns cached state, not actual pin reading
         */
        PinState getCurrentState() const { return currentState_; }

        // Operator overloads for convenience
        /**
         * @brief Assignment operator for setting pin state
         * @param state Logic level to set
         * @return Reference to this object for chaining
         * 
         * Allows syntax like: led = PinState::HIGH;
         */
        GPIOOutput& operator=(PinState state);
        
        /**
         * @brief Conversion operator to PinState
         * @return Current output state
         * 
         * Allows implicit conversion to PinState for comparisons.
         */
        operator PinState() const { return currentState_; }
    };

    /**
     * @class GPIOInput
     * @brief GPIO Input pin class with input-specific functionality
     * 
     * Specialized GPIO class for input operations. Provides methods for
     * reading digital values and configuring pull resistors.
     * Optimized for polling-based input reading.
     * 
     * @note Pin is automatically configured as INPUT mode
     * @see GPIOBase, GPIOEXTI
     */
    class GPIOInput : public GPIOBase
    {
    public:
        /**
         * @brief Construct a new GPIOInput object
         * 
         * Creates an input pin with specified pull resistor configuration.
         * The pin is automatically configured as INPUT mode.
         * 
         * @param port GPIO port (GPIOA, GPIOB, etc.)
         * @param pin GPIO pin number (0-15)
         * @param pull Pull resistor configuration (NO_PULL, PULL_UP, PULL_DOWN)
         * 
         * @warning Pin number must be in range 0-15
         */
        GPIOInput(GPIO_TypeDef* port, uint32_t pin, PinPull pull = PinPull::NO_PULL);

        /**
         * @brief Construct a new GPIOInput object with full configuration
         * 
         * Creates an input pin using a complete configuration structure.
         * Mode is automatically set to INPUT regardless of config.mode value.
         * 
         * @param config Complete pin configuration structure
         * @note config.mode is forced to INPUT
         */
        GPIOInput(const PinConfig& config);

        // Override base class methods
        /**
         * @brief Validate configuration for input mode
         * @return true if pin is configured as INPUT
         */
        bool isValidForMode() const override;

        // Input-specific methods
        /**
         * @brief Read the current logic level of the input pin
         * @return Current pin state (HIGH or LOW)
         * 
         * Reads the actual voltage level on the pin and returns
         * the corresponding logic level.
         */
        PinState read() const;
        
        /**
         * @brief Check if the input pin is at logic HIGH
         * @return true if pin is HIGH, false if LOW
         * 
         * Convenience method equivalent to (read() == PinState::HIGH).
         */
        bool isHigh() const;
        
        /**
         * @brief Check if the input pin is at logic LOW
         * @return true if pin is LOW, false if HIGH
         * 
         * Convenience method equivalent to (read() == PinState::LOW).
         */
        bool isLow() const;

        // Operator overloads for convenience
        /**
         * @brief Conversion operator to bool
         * @return true if pin is HIGH, false if LOW
         * 
         * Allows using the pin in boolean context: if (button) {...}
         */
        operator bool() const { return isHigh(); }
        
        /**
         * @brief Conversion operator to PinState
         * @return Current pin state
         * 
         * Allows implicit conversion to PinState for comparisons.
         */
        operator PinState() const { return read(); }
    };

    /**
     * @class GPIOEXTI
     * @brief GPIO External Interrupt pin class with interrupt functionality
     * 
     * Specialized GPIO class for external interrupt operations. Inherits from GPIOInput
     * and adds interrupt configuration, callback management, and NVIC integration.
     * Supports edge-triggered interrupts with configurable trigger conditions.
     * 
     * @note Pin is automatically configured as INPUT mode with EXTI functionality
     * @warning Only one callback per EXTI line (pin number) is supported
     * @see GPIOInput, GPIOBase
     */
    class GPIOEXTI : public GPIOInput
    {
    private:
        static InterruptCallback callbacks_[16];  ///< Static callback array for all EXTI lines (0-15)
        EXTITrigger trigger_;           ///< Current interrupt trigger configuration
        InterruptCallback callback_;    ///< User callback function for this pin
        bool interruptEnabled_;         ///< Current interrupt enable state

        /**
         * @brief Get EXTI line mask from pin number
         * 
         * Converts pin number to EXTI line mask for LL driver functions.
         * 
         * @param pin Pin number (0-15)
         * @return EXTI line mask (1 << pin)
         */
        uint32_t getEXTILine(uint32_t pin) const;

        /**
         * @brief Configure EXTI hardware registers
         * 
         * Sets up the EXTI line, SYSCFG routing, and trigger configuration.
         * Called automatically during initialization.
         */
        void configureEXTI();

    public:
        /**
         * @brief Construct a new GPIOEXTI object
         * 
         * Creates an interrupt-capable input pin with specified trigger and pull configuration.
         * EXTI hardware is configured but interrupt is not enabled until enableInterrupt() is called.
         * 
         * @param port GPIO port (GPIOA, GPIOB, etc.)
         * @param pin GPIO pin number (0-15)
         * @param trigger Interrupt trigger condition (RISING, FALLING, or RISING_FALLING)
         * @param pull Pull resistor configuration (NO_PULL, PULL_UP, PULL_DOWN)
         * 
         * @warning Pin number must be in range 0-15
         * @note Interrupt is configured but not enabled until enableInterrupt() is called
         */
        GPIOEXTI(GPIO_TypeDef* port, uint32_t pin, 
                 EXTITrigger trigger = EXTITrigger::RISING,
                 PinPull pull = PinPull::NO_PULL);

        /**
         * @brief Construct a new GPIOEXTI object with full configuration
         * 
         * Creates an interrupt pin using a complete configuration structure.
         * 
         * @param config Complete pin configuration structure
         * @note config.trigger is used for interrupt configuration
         */
        GPIOEXTI(const PinConfig& config);

        /**
         * @brief Destructor - unregisters instance from static registry
         * 
         * Ensures the GPIOEXTI instance is removed from the interrupt registry
         * when the object is destroyed.
         */
        ~GPIOEXTI();

        // Override base class methods
        /**
         * @brief Validate configuration for interrupt mode
         * @return true if pin is configured as INPUT (required for EXTI)
         */
        bool isValidForMode() const override;

        // EXTI-specific methods
        /**
         * @brief Enable the external interrupt for this pin
         * 
         * Enables the NVIC interrupt and registers the callback function.
         * The pin must have a valid callback set before enabling.
         * 
         * @warning Ensure callback is set before enabling interrupt
         * @see setCallback()
         */
        void enableInterrupt();
        
        /**
         * @brief Disable the external interrupt for this pin
         * 
         * Disables the EXTI line interrupt and clears the callback registration.
         * Does not disable the entire NVIC interrupt line (other pins may use it).
         */
        void disableInterrupt();
        
        /**
         * @brief Set the interrupt callback function
         * 
         * Registers a function to be called when the interrupt occurs.
         * The callback should be fast and non-blocking as it runs in interrupt context.
         * 
         * @param callback Function to call on interrupt (lambda, function pointer, etc.)
         * 
         * @warning Callback runs in interrupt context - keep it short and fast
         * @note Use std::function, lambdas, or function pointers
         */
        void setCallback(InterruptCallback callback);
        
        /**
         * @brief Change the interrupt trigger condition
         * 
         * Modifies when the interrupt will be triggered. Changes are applied immediately.
         * 
         * @param trigger New trigger condition (RISING, FALLING, RISING_FALLING)
         */
        void setTrigger(EXTITrigger trigger);
        
        /**
         * @brief Check if interrupt is pending for this pin
         * @return true if interrupt flag is set, false otherwise
         * 
         * Useful for polling-based interrupt handling or debugging.
         */
        bool isInterruptPending() const;
        
        /**
         * @brief Clear the interrupt pending flag
         * 
         * Manually clears the EXTI pending flag for this pin.
         * Normally handled automatically by the interrupt system.
         */
        void clearInterruptFlag();

        // Static interrupt handler (to be called from ISR)
        /**
         * @brief Static interrupt handler for all EXTI pins
         * 
         * Central interrupt handler that dispatches to individual pin callbacks.
         * This method should be called from your EXTI interrupt service routines.
         * 
         * @param pin Pin number (0-15) that triggered the interrupt
         * 
         * @note Call this from your EXTIx_IRQHandler functions
         * @see gpio_example.cpp for ISR implementation examples
         */
        static void handleInterrupt(uint32_t pin);

        // Getters
        /**
         * @brief Check if interrupt is currently enabled
         * @return true if interrupt is enabled, false otherwise
         */
        bool isInterruptEnabled() const { return interruptEnabled_; }
        
        /**
         * @brief Get the current interrupt trigger configuration
         * @return Current trigger type (RISING, FALLING, or RISING_FALLING)
         */
        EXTITrigger getTrigger() const { return trigger_; }
    };

} // namespace GPIO

#endif /* DEVICE_INC_GPIO_H_ */
