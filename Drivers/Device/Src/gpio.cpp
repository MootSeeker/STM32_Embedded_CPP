/**
 * @file gpio.cpp
 * @brief Implementation of intelligent GPIO class library for STM32L4xx using LL drivers
 * 
 * @author kevin
 * @date Sep 16, 2025
 * @version 1.0
 * 
 * This file implements the GPIO class hierarchy providing object-oriented access
 * to STM32L4xx GPIO peripherals. Uses STM32 Low-Level (LL) drivers for optimal
 * performance while maintaining C++ abstraction benefits.
 * 
 * @note Requires STM32L4xx LL drivers and proper system clock configuration
 * @see gpio.h for class declarations and documentation
 */

#include "gpio.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_system.h"

namespace GPIO
{
//=============================================================================
// GPIOBase Implementation
//=============================================================================

/**
 * @brief Construct GPIO pin with basic parameters
 * 
 * Initializes GPIO configuration structure with provided parameters
 * and sets remaining parameters to safe defaults.
 * 
 * @param port GPIO port (GPIOA, GPIOB, etc.)
 * @param pin GPIO pin number (0-15)
 * @param mode Pin mode (INPUT, OUTPUT, ALTERNATE, ANALOG)
 */
GPIOBase::GPIOBase(GPIO_TypeDef* port, uint32_t pin, PinMode mode)
    : port_(port), pin_(pin) 
{
    config_.port = port;
    config_.pin = pin;
    config_.mode = mode;
    config_.outputType = PinOutputType::PUSH_PULL;
    config_.speed = PinSpeed::LOW;
    config_.pull = PinPull::NO_PULL;
    config_.alternate = 0;
    config_.trigger = EXTITrigger::RISING;
}

/**
 * @brief Construct GPIO pin with full configuration
 * 
 * Initializes GPIO pin using a complete configuration structure.
 * All parameters are taken from the provided configuration.
 * 
 * @param config Complete GPIO pin configuration structure
 */
GPIOBase::GPIOBase(const PinConfig& config)
    : port_(config.port), pin_(config.pin), config_(config) 
{
}


/**
 * @brief Configure GPIO pin speed/slew rate
 * 
 * Sets the output speed which affects rise/fall times and power consumption.
 * Higher speeds provide faster switching but increase power consumption
 * and electromagnetic emissions.
 * 
 * @param speed Pin speed setting (LOW, MEDIUM, HIGH, VERY_HIGH)
 * @note Affects output pins only, no effect on input pins
 */
void GPIOBase::setSpeed(PinSpeed speed) {
    config_.speed = speed;
    LL_GPIO_SetPinSpeed(port_, getLLPin(pin_), static_cast<uint32_t>(speed));
}

/**
 * @brief Configure internal pull resistor
 * 
 * Sets the internal pull-up or pull-down resistor for the pin.
 * Pull resistors are typically ~40kΩ and help ensure defined logic
 * levels when external signals are not driving the pin.
 * 
 * @param pull Pull resistor configuration (NO_PULL, PULL_UP, PULL_DOWN)
 */
void GPIOBase::setPull(PinPull pull) {
    config_.pull = pull;
    LL_GPIO_SetPinPull(port_, getLLPin(pin_), static_cast<uint32_t>(pull));
}


/**
 * @brief Configure hardware registers according to current settings
 * 
 * Applies all configuration parameters to the actual GPIO hardware registers.
 * This includes enabling port clock, setting mode, speed, pull resistors,
 * output type, and alternate function if applicable.
 * 
 * @note This is called automatically by constructors and setConfig()
 */
void GPIOBase::configureHardware() {
    // Enable GPIO port clock
    enablePortClock();
    
    // Use LL_GPIO_Init for proper GPIO initialization
    LL_GPIO_InitTypeDef GPIO_InitStruct = {};
    GPIO_InitStruct.Pin = getLLPin(pin_);
    GPIO_InitStruct.Mode = static_cast<uint32_t>(config_.mode);
    GPIO_InitStruct.Speed = static_cast<uint32_t>(config_.speed);
    GPIO_InitStruct.OutputType = static_cast<uint32_t>(config_.outputType);
    GPIO_InitStruct.Pull = static_cast<uint32_t>(config_.pull);
    GPIO_InitStruct.Alternate = config_.alternate;
    
    LL_GPIO_Init(port_, &GPIO_InitStruct);
}

/**
 * @brief Enable clock for the GPIO port
 * 
 * Enables the peripheral clock for the GPIO port to allow register access.
 * Each port has its own clock enable bit in the RCC peripheral.
 * 
 * @note Called automatically during hardware configuration
 */
void GPIOBase::enablePortClock() {
    if (port_ == GPIOA) {
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    } else if (port_ == GPIOB) {
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    } else if (port_ == GPIOC) {
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
    } else if (port_ == GPIOD) {
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);
    } else if (port_ == GPIOE) {
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE);
    } else if (port_ == GPIOH) {
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOH);
    }
}

/**
 * @brief Convert pin number to LL driver pin mask
 * 
 * Converts a pin number (0-15) to the corresponding bit mask
 * used by the STM32 LL drivers for register operations.
 * 
 * @param pin Pin number (0-15)
 * @return Corresponding LL pin mask (LL_GPIO_PIN_x)
 */
uint32_t GPIOBase::getLLPin(uint32_t pin) const {
    return (1U << pin);
}

//=============================================================================
// GPIOOutput Implementation
//=============================================================================

/**
 * @brief Construct output pin with output type configuration
 * 
 * Creates a GPIO output pin with specified output driver type.
 * Pin is configured for OUTPUT mode and hardware is set up immediately.
 * Initial state is set to LOW for safe startup.
 * 
 * @param port GPIO port (GPIOA, GPIOB, etc.)
 * @param pin GPIO pin number (0-15)
 * @param outputType Output driver type (PUSH_PULL or OPEN_DRAIN)
 * @param speed Pin speed setting (LOW, MEDIUM, HIGH, VERY_HIGH)
 */
GPIOOutput::GPIOOutput(GPIO_TypeDef* port, uint32_t pin, 
                       PinOutputType outputType, PinSpeed speed)
    : GPIOBase(port, pin, PinMode::OUTPUT) 
{
    config_.outputType = outputType;
    config_.speed = speed;
    init();
}

/**
 * @brief Construct output pin with full configuration
 * 
 * Creates a GPIO output pin using a complete configuration structure.
 * Pin mode is forced to OUTPUT regardless of config parameter.
 * 
 * @param config Pin configuration (mode will be overridden to OUTPUT)
 */
GPIOOutput::GPIOOutput(const PinConfig& config) 
    : GPIOBase(config) 
{
    config_.mode = PinMode::OUTPUT; // Force OUTPUT mode
    init();
}

/**
 * @brief Initialize output pin hardware and state
 * 
 * Configures GPIO hardware registers and sets initial output state to LOW.
 * This ensures a defined startup state and prevents glitches.
 */
void GPIOOutput::init() {
    configureHardware();
    currentState_ = PinState::LOW;
    reset(); // Start with low state
}

/**
 * @brief Validate that pin is configured for output mode
 * 
 * @return true if pin mode is OUTPUT, false otherwise
 */
bool GPIOOutput::isValidForMode() const {
    return (config_.mode == PinMode::OUTPUT);
}

/**
 * @brief Write logic level to output pin
 * 
 * Sets the output pin to the specified logic level using atomic
 * register operations. Updates internal state tracking.
 * 
 * @param state Logic level to output (HIGH or LOW)
 */
void GPIOOutput::write(PinState state) {
    currentState_ = state;
    if (state == PinState::HIGH) {
        LL_GPIO_SetOutputPin(port_, getLLPin(pin_));
    } else {
        LL_GPIO_ResetOutputPin(port_, getLLPin(pin_));
    }
}

/**
 * @brief Set output pin to logic HIGH level
 * 
 * Drives the pin to VDD voltage level using atomic register operation.
 * For push-pull outputs, actively drives HIGH. For open-drain outputs,
 * releases pin to float HIGH via external pull-up.
 */
void GPIOOutput::set() {
    write(PinState::HIGH);
}

/**
 * @brief Set output pin to logic LOW level
 * 
 * Drives the pin to VSS (ground) voltage level using atomic register operation.
 * Works identically for both push-pull and open-drain output types.
 */
void GPIOOutput::reset() {
    write(PinState::LOW);
}

/**
 * @brief Toggle output pin state
 * 
 * Changes pin from HIGH to LOW or LOW to HIGH atomically.
 * Uses hardware toggle register for glitch-free operation.
 * Updates internal state tracking.
 */
void GPIOOutput::toggle() {
    currentState_ = (currentState_ == PinState::HIGH) ? PinState::LOW : PinState::HIGH;
    LL_GPIO_TogglePin(port_, getLLPin(pin_));
}

/**
 * @brief Assignment operator for pin state
 * 
 * Allows direct assignment of logic levels to the pin object.
 * Provides convenient syntax: pin = PinState::HIGH;
 * 
 * @param state Logic level to assign
 * @return Reference to this object for chaining
 */
GPIOOutput& GPIOOutput::operator=(PinState state) {
    write(state);
    return *this;
}

//=============================================================================
// GPIOInput Implementation
//=============================================================================

/**
 * @brief Construct input pin with pull resistor configuration
 * 
 * Creates a GPIO input pin with specified internal pull resistor.
 * Pin is configured for INPUT mode and hardware is set up immediately.
 * 
 * @param port GPIO port (GPIOA, GPIOB, etc.)
 * @param pin GPIO pin number (0-15)
 * @param pull Internal pull resistor setting (NO_PULL, PULL_UP, PULL_DOWN)
 */
GPIOInput::GPIOInput(GPIO_TypeDef* port, uint32_t pin, PinPull pull)
    : GPIOBase(port, pin, PinMode::INPUT) 
{
    config_.pull = pull;
    init();
}

/**
 * @brief Construct input pin with full configuration
 * 
 * Creates a GPIO input pin using a complete configuration structure.
 * Pin mode is forced to INPUT regardless of config parameter.
 * 
 * @param config Pin configuration (mode will be overridden to INPUT)
 */
GPIOInput::GPIOInput(const PinConfig& config) 
    : GPIOBase(config) 
{
    config_.mode = PinMode::INPUT; // Force INPUT mode
    init();
}

/**
 * @brief Initialize input pin hardware
 * 
 * Configures GPIO hardware registers according to current settings.
 * No additional initialization needed beyond hardware configuration.
 */
void GPIOInput::init() {
    configureHardware();
}

/**
 * @brief Validate that pin is configured for input mode
 * 
 * @return true if pin mode is INPUT, false otherwise
 */
bool GPIOInput::isValidForMode() const {
    return (config_.mode == PinMode::INPUT);
}

/**
 * @brief Read current logic level of input pin
 * 
 * Samples the actual pin voltage and converts to digital logic level.
 * Returns HIGH for voltages above input threshold (~1.65V for 3.3V operation),
 * LOW for voltages below threshold.
 * 
 * @return Current pin state (HIGH or LOW)
 */
PinState GPIOInput::read() const {
    return LL_GPIO_IsInputPinSet(port_, getLLPin(pin_)) ? PinState::HIGH : PinState::LOW;
}

/**
 * @brief Check if input pin is at logic HIGH level
 * 
 * @return true if pin voltage is above input threshold, false otherwise
 */
bool GPIOInput::isHigh() const {
    return (read() == PinState::HIGH);
}

/**
 * @brief Check if input pin is at logic LOW level
 * 
 * @return true if pin voltage is below input threshold, false otherwise
 */
bool GPIOInput::isLow() const {
    return (read() == PinState::LOW);
}

//=============================================================================
// GPIOEXTI Implementation
//=============================================================================

/**
 * @brief Construct interrupt-capable input pin
 * 
 * Creates a GPIO input pin capable of generating external interrupts.
 * Configures EXTI hardware but leaves interrupt disabled until explicitly enabled.
 * 
 * @param port GPIO port (GPIOA, GPIOB, etc.)
 * @param pin GPIO pin number (0-15)
 * @param trigger Interrupt trigger condition (RISING, FALLING, BOTH)
 * @param pull Internal pull resistor setting (NO_PULL, PULL_UP, PULL_DOWN)
 */
GPIOEXTI::GPIOEXTI(GPIO_TypeDef* port, uint32_t pin, 
                   EXTITrigger trigger, PinPull pull)
    : GPIOInput(port, pin, pull), trigger_(trigger), 
      callback_(nullptr), interruptEnabled_(false) 
{
    config_.trigger = trigger;
    init();
}

/**
 * @brief Construct interrupt-capable pin with full configuration
 * 
 * Creates a GPIO interrupt pin using complete configuration structure.
 * Uses trigger setting from configuration for EXTI setup.
 * 
 * @param config Complete pin configuration including trigger setting
 */
GPIOEXTI::GPIOEXTI(const PinConfig& config) 
    : GPIOInput(config), trigger_(config.trigger), 
      callback_(nullptr), interruptEnabled_(false) 
{
    init();
}

/**
 * @brief Initialize EXTI hardware configuration
 * 
 * Configures EXTI line, trigger condition, and port connection.
 * Enables SYSCFG clock for EXTI port multiplexing.
 * Interrupt remains disabled until enableInterrupt() is called.
 */
void GPIOEXTI::init() {
    GPIOInput::init(); // Initialize base input functionality
    configureEXTI();
}

/**
 * @brief Configure EXTI hardware registers
 * 
 * Sets up External Interrupt/Event Controller for this pin.
 * Configures trigger condition, enables EXTI line, and connects
 * GPIO port to EXTI input multiplexer.
 * 
 * @note SYSCFG clock must be enabled for port connection
 */
void GPIOEXTI::configureEXTI() {
    // Enable SYSCFG clock for EXTI configuration
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    
    // Configure EXTI line
    LL_EXTI_InitTypeDef EXTI_InitStruct = {};
    EXTI_InitStruct.Line_0_31 = getEXTILine(pin_);
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = static_cast<uint32_t>(trigger_);
    LL_EXTI_Init(&EXTI_InitStruct);
    
    // Connect GPIO port to EXTI input
    uint32_t extiLine = LL_SYSCFG_EXTI_LINE0;
    switch (pin_) {
        case 0: extiLine = LL_SYSCFG_EXTI_LINE0; break;
        case 1: extiLine = LL_SYSCFG_EXTI_LINE1; break;
        case 2: extiLine = LL_SYSCFG_EXTI_LINE2; break;
        case 3: extiLine = LL_SYSCFG_EXTI_LINE3; break;
        case 4: extiLine = LL_SYSCFG_EXTI_LINE4; break;
        case 5: extiLine = LL_SYSCFG_EXTI_LINE5; break;
        case 6: extiLine = LL_SYSCFG_EXTI_LINE6; break;
        case 7: extiLine = LL_SYSCFG_EXTI_LINE7; break;
        case 8: extiLine = LL_SYSCFG_EXTI_LINE8; break;
        case 9: extiLine = LL_SYSCFG_EXTI_LINE9; break;
        case 10: extiLine = LL_SYSCFG_EXTI_LINE10; break;
        case 11: extiLine = LL_SYSCFG_EXTI_LINE11; break;
        case 12: extiLine = LL_SYSCFG_EXTI_LINE12; break;
        case 13: extiLine = LL_SYSCFG_EXTI_LINE13; break;
        case 14: extiLine = LL_SYSCFG_EXTI_LINE14; break;
        case 15: extiLine = LL_SYSCFG_EXTI_LINE15; break;
        default: extiLine = LL_SYSCFG_EXTI_LINE0; break;
    }
    
    if (port_ == GPIOA) {
        LL_SYSCFG_SetEXTISource(extiLine, LL_SYSCFG_EXTI_PORTA);
    } else if (port_ == GPIOB) {
        LL_SYSCFG_SetEXTISource(extiLine, LL_SYSCFG_EXTI_PORTB);
    } else if (port_ == GPIOC) {
        LL_SYSCFG_SetEXTISource(extiLine, LL_SYSCFG_EXTI_PORTC);
    } else if (port_ == GPIOD) {
        LL_SYSCFG_SetEXTISource(extiLine, LL_SYSCFG_EXTI_PORTD);
    } else if (port_ == GPIOE) {
        LL_SYSCFG_SetEXTISource(extiLine, LL_SYSCFG_EXTI_PORTE);
    } else if (port_ == GPIOH) {
        LL_SYSCFG_SetEXTISource(extiLine, LL_SYSCFG_EXTI_PORTH);
    }
}

/**
 * @brief Enable interrupt generation for this pin
 * 
 * Enables the corresponding NVIC interrupt channel and marks
 * interrupt as enabled. Pin will generate interrupts according
 * to configured trigger condition.
 * 
 * @warning Ensure interrupt handler is properly configured before enabling
 */
void GPIOEXTI::enableInterrupt() {
    if (!interruptEnabled_) {
        IRQn_Type irqn;
        
        // Determine NVIC interrupt channel based on pin number
        if (pin_ == 0) irqn = EXTI0_IRQn;
        else if (pin_ == 1) irqn = EXTI1_IRQn;
        else if (pin_ == 2) irqn = EXTI2_IRQn;
        else if (pin_ == 3) irqn = EXTI3_IRQn;
        else if (pin_ == 4) irqn = EXTI4_IRQn;
        else if (pin_ >= 5 && pin_ <= 9) irqn = EXTI9_5_IRQn;
        else if (pin_ >= 10 && pin_ <= 15) irqn = EXTI15_10_IRQn;
        else return; // Invalid pin number
        
        NVIC_EnableIRQ(irqn);
        interruptEnabled_ = true;
    }
}

/**
 * @brief Disable interrupt generation for this pin
 * 
 * Disables the NVIC interrupt channel for this pin.
 * Pin will no longer generate interrupts until re-enabled.
 * 
 * @note Other pins sharing the same IRQ channel may still generate interrupts
 */
void GPIOEXTI::disableInterrupt() {
    if (interruptEnabled_) {
        IRQn_Type irqn;
        
        if (pin_ == 0) irqn = EXTI0_IRQn;
        else if (pin_ == 1) irqn = EXTI1_IRQn;
        else if (pin_ == 2) irqn = EXTI2_IRQn;
        else if (pin_ == 3) irqn = EXTI3_IRQn;
        else if (pin_ == 4) irqn = EXTI4_IRQn;
        else if (pin_ >= 5 && pin_ <= 9) irqn = EXTI9_5_IRQn;
        else if (pin_ >= 10 && pin_ <= 15) irqn = EXTI15_10_IRQn;
        else return;
        
        NVIC_DisableIRQ(irqn);
        interruptEnabled_ = false;
    }
}

/**
 * @brief Set interrupt callback function
 * 
 * Registers a callback function to be executed when interrupt occurs.
 * Callback should be lightweight and avoid blocking operations.
 * 
 * @param callback Function to call on interrupt (nullptr to clear)
 * @warning Callback executes in interrupt context - keep it fast!
 */
void GPIOEXTI::setCallback(std::function<void()> callback) {
    callback_ = callback;
}

/**
 * @brief Static interrupt handler for all EXTI pins
 * 
 * Central interrupt handler that dispatches to individual pin callbacks.
 * This method should be called from your EXTI interrupt service routines.
 * 
 * @param pin Pin number (0-15) that triggered the interrupt
 * @note Call this from your EXTIx_IRQHandler functions
 */
void GPIOEXTI::handleInterrupt(uint32_t pin) {
    uint32_t extiLine = (1U << pin);
    if (LL_EXTI_IsActiveFlag_0_31(extiLine)) {
        LL_EXTI_ClearFlag_0_31(extiLine);
        
        // Find the GPIOEXTI instance for this pin and call its callback
        // Note: This requires a static registry of GPIOEXTI instances
        // For now, this is a basic implementation
    }
}

/**
 * @brief Get EXTI line number for pin
 * 
 * Converts GPIO pin number to corresponding EXTI line mask.
 * Each GPIO pin connects to its corresponding EXTI line.
 * 
 * @param pin GPIO pin number (0-15)
 * @return EXTI line mask for LL driver functions
 */
uint32_t GPIOEXTI::getEXTILine(uint32_t pin) const {
    return (1U << pin);
}

}

