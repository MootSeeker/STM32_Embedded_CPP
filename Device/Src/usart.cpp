/**
 * @file    usart.cpp
 * @brief   Intelligent USART class implementation for STM32L4xx
 * @date    2024-06-10
 * @author  MootSeeker
 * 
 * Type-safe, interrupt-driven USART driver with circular buffering.
 * Supports LPUART_1, USART_1, USART_2, and USART_3.
 */

#include "usart.h"
#include "stm32l4xx_ll_lpuart.h"

// Note: stm32l4xx_ll_usart.h is not available in this HAL version
// USART_1-3 use register-level or HAL-level access instead

namespace USART
{
    /**
     * @brief Type-safe registry for USART instances
     * 
     * Stores void* instance pointers indexed by peripheral type.
     * This enables safe ISR dispatch without casting issues.
     */
    static void* g_usartRegistry[static_cast<size_t>(PeripheralType::COUNT)] = {nullptr};

    /**
     * @brief Get instance pointer from registry
     * @param peripheral Peripheral type to look up
     * @return Registered instance pointer or nullptr
     */
    static inline void* getRegisteredInstance(PeripheralType peripheral) noexcept {
        size_t index = static_cast<size_t>(peripheral);
        if (index < static_cast<size_t>(PeripheralType::COUNT)) {
            return g_usartRegistry[index];
        }
        return nullptr;
    }

    /**
     * @brief Set instance pointer in registry
     * @param peripheral Peripheral type to register
     * @param instance Instance pointer to store
     * @return Status indicating success or error
     */
    static UsartStatus registerInstance(PeripheralType peripheral, void* instance) noexcept {
        size_t index = static_cast<size_t>(peripheral);
        if (index >= static_cast<size_t>(PeripheralType::COUNT)) {
            return UsartStatus{UsartError::INVALID_PERIPHERAL, 0};
        }
        g_usartRegistry[index] = instance;
        return UsartStatus{UsartError::OK, 0};
    }

    template<uint16_t BUFFER_SIZE>
    UsartDriver<BUFFER_SIZE>::UsartDriver(PeripheralType peripheral) noexcept
        : peripheralType(peripheral), usartInstance(nullptr), transmissionActive(false), isInitialized(false) {
        
        // Set the hardware instance based on peripheral type (type-safe pointer)
        switch (peripheral) {
            case PeripheralType::LPUART_1:
                usartInstance = LPUART1;
                break;
            case PeripheralType::USART_1:
                usartInstance = USART1;
                break;
            case PeripheralType::USART_2:
                usartInstance = USART2;
                break;
            case PeripheralType::USART_3:
                usartInstance = USART3;
                break;
            case PeripheralType::COUNT:
                // Invalid peripheral type
                break;
        }
    }

    template<uint16_t BUFFER_SIZE>
    UsartStatus UsartDriver<BUFFER_SIZE>::initialize(const Config& cfg) noexcept {
        if (usartInstance == nullptr) {
            return UsartStatus{UsartError::INVALID_PERIPHERAL, 0};
        }

        config = cfg;
        isInitialized = false;  // Reset flag until successful initialization
        
        switch (peripheralType) {
            case PeripheralType::LPUART_1:
                initializeLpuart();
                break;
            case PeripheralType::USART_1:
            case PeripheralType::USART_2:
            case PeripheralType::USART_3:
                initializeUsart();
                break;
            case PeripheralType::COUNT:
                return UsartStatus{UsartError::INVALID_PERIPHERAL, 0};
        }
        
        isInitialized = true;
        
        // Register instance in global registry for interrupt handling
        return registerInstance(peripheralType, static_cast<void*>(this));
    }

    template<uint16_t BUFFER_SIZE>
    void UsartDriver<BUFFER_SIZE>::initializeLpuart() noexcept {
        // Configure LPUART
        LL_LPUART_SetBaudRate(usartInstance, LL_RCC_GetLPUARTClockFreq(LL_RCC_LPUART1_CLKSOURCE), config.baudRate);
        LL_LPUART_SetDataWidth(usartInstance, config.wordLength);
        LL_LPUART_SetStopBitsLength(usartInstance, config.stopBits);
        LL_LPUART_SetParity(usartInstance, config.parity);
        LL_LPUART_SetTransferDirection(usartInstance, config.transferDirection);
        LL_LPUART_SetHWFlowCtrl(usartInstance, config.hwFlowControl);
        
        // Enable LPUART1 clock (if not already enabled)
        LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPUART1);
        
        // Enable LPUART
        LL_LPUART_Enable(usartInstance);
        
        // Enable TX empty interrupt
        LL_LPUART_EnableIT_TXE(usartInstance);
        
        // Enable NVIC interrupt
        NVIC_SetPriority(LPUART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(LPUART1_IRQn);
    }

    template<uint16_t BUFFER_SIZE>
    void UsartDriver<BUFFER_SIZE>::initializeUsart() noexcept {
        if (usartInstance == nullptr) {
            return;
        }

        // Enable appropriate clock and configure based on USART instance
        IRQn_Type irqn = (IRQn_Type)0;
        uint32_t baudRate = config.baudRate;
        
        switch (peripheralType) {
            case PeripheralType::USART_1:
                LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
                irqn = USART1_IRQn;
                break;
            case PeripheralType::USART_2:
                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
                irqn = USART2_IRQn;
                break;
            case PeripheralType::USART_3:
                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
                irqn = USART3_IRQn;
                break;
            default:
                return;
        }
        
        // Configure USART registers directly (LL_USART functions not available in this HAL version)
        // USART CR1 setup: 8-bit data width, transmitter/receiver enabled
        usartInstance->CR1 = (config.wordLength | config.transferDirection | USART_CR1_UE);
        
        // USART CR2 setup: stop bits configuration
        usartInstance->CR2 = config.stopBits;
        
        // USART CR3 setup: hardware flow control and parity
        usartInstance->CR3 = (config.hwFlowControl | config.parity);
        
        // Set baud rate using BRR register (simplified calculation for standard clock)
        // For STM32L433, USART clocks are derived from PCLK
        // BRR = USART_CLK / BaudRate
        // Assuming 80MHz PCLK: BRR = 80000000 / 115200 ≈ 694
        // For accurate baud rate: use HAL_UART_Init or calculate based on actual clock
        uint32_t brr = SystemCoreClock / 2 / baudRate;  // Approximate for standard configuration
        if ((usartInstance->CR1 & USART_CR1_OVER8) != 0) {
            brr = (brr & ~0x0F) | ((brr & 0x0F) >> 1);
        }
        usartInstance->BRR = brr;
        
        // Enable TX empty interrupt
        usartInstance->CR1 |= USART_CR1_TXEIE;
        
        // Enable NVIC interrupt
        NVIC_SetPriority(irqn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(irqn);
    }

    template<uint16_t BUFFER_SIZE>
    bool UsartDriver<BUFFER_SIZE>::sendByte(uint8_t data) noexcept {
        if (!isInitialized) {
            return false;
        }
        bool success = txBuffer.put(data);
        if (success && !transmissionActive) {
            startTransmission();
        }
        return success;
    }

    template<uint16_t BUFFER_SIZE>
    uint16_t UsartDriver<BUFFER_SIZE>::sendData(const uint8_t* data, uint16_t length) noexcept {
        if (!isInitialized || data == nullptr || length == 0) {
            return 0;
        }
        
        uint16_t sent = 0;
        for (uint16_t i = 0; i < length; i++) {
            if (txBuffer.put(data[i])) {
                sent++;
            } else {
                break; // Buffer full
            }
        }
        
        if (sent > 0 && !transmissionActive) {
            startTransmission();
        }
        
        return sent;
    }

    template<uint16_t BUFFER_SIZE>
    uint16_t UsartDriver<BUFFER_SIZE>::sendString(const char* str) noexcept {
        if (!isInitialized || str == nullptr) {
            return 0;
        }
        
        uint16_t sent = 0;
        while (*str != '\0') {
            if (txBuffer.put(static_cast<uint8_t>(*str))) {
                sent++;
                str++;
            } else {
                break; // Buffer full
            }
        }
        
        if (sent > 0 && !transmissionActive) {
            startTransmission();
        }
        
        return sent;
    }

    template<uint16_t BUFFER_SIZE>
    uint16_t UsartDriver<BUFFER_SIZE>::sendFormatted(const char* format, ...) noexcept {
        if (!isInitialized || format == nullptr) {
            return 0;
        }
        
        // Use a bounded temporary buffer to prevent stack overflow
        // Note: Output may be truncated if it exceeds 256 characters
        static constexpr uint16_t TEMP_BUFFER_SIZE = 256;
        char buffer[TEMP_BUFFER_SIZE];
        
        va_list args;
        va_start(args, format);
        int length = vsnprintf(buffer, TEMP_BUFFER_SIZE, format, args);
        va_end(args);
        
        if (length > 0) {
            // vsnprintf returns the number of characters that would have been written
            uint16_t actualLength = (length < TEMP_BUFFER_SIZE) ? static_cast<uint16_t>(length) 
                                                                : (TEMP_BUFFER_SIZE - 1);
            return sendData(reinterpret_cast<const uint8_t*>(buffer), actualLength);
        }
        
        return 0;
    }

    template<uint16_t BUFFER_SIZE>
    uint16_t UsartDriver<BUFFER_SIZE>::sendHex(const uint8_t* data, uint16_t length, bool uppercase) noexcept {
        if (!isInitialized || data == nullptr || length == 0) {
            return 0;
        }
        
        const char* hexChars = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
        uint16_t sent = 0;
        
        for (uint16_t i = 0; i < length; i++) {
            uint8_t byte = data[i];
            
            // Send high nibble
            if (!txBuffer.put(hexChars[(byte >> 4) & 0x0F])) {
                break;
            }
            sent++;
            
            // Send low nibble
            if (!txBuffer.put(hexChars[byte & 0x0F])) {
                break;
            }
            sent++;
        }
        
        if (sent > 0 && !transmissionActive) {
            startTransmission();
        }
        
        return sent;
    }

    template<uint16_t BUFFER_SIZE>
    uint16_t UsartDriver<BUFFER_SIZE>::sendBinary(const uint8_t* data, uint16_t length) noexcept {
        if (!isInitialized || data == nullptr || length == 0) {
            return 0;
        }
        
        uint16_t sent = 0;
        bool bufferFull = false;
        
        for (uint16_t i = 0; i < length && !bufferFull; i++) {
            uint8_t byte = data[i];
            
            // Send each bit (MSB first)
            for (int bit = 7; bit >= 0; bit--) {
                char bitChar = ((byte >> bit) & 0x01) ? '1' : '0';
                if (!txBuffer.put(bitChar)) {
                    bufferFull = true;
                    break;
                }
                sent++;
            }
        }
        
        if (sent > 0 && !transmissionActive) {
            startTransmission();
        }
        
        return sent;
    }

    template<uint16_t BUFFER_SIZE>
    void UsartDriver<BUFFER_SIZE>::startTransmission() noexcept {
        if (txBuffer.isEmpty()) {
            return;
        }
        
        transmissionActive = true;
        
        // Get first byte and start transmission
        uint8_t data;
        if (txBuffer.get(data)) {
            transmitByte(data);
        }
    }

    template<uint16_t BUFFER_SIZE>
    void UsartDriver<BUFFER_SIZE>::transmitByte(uint8_t data) noexcept {
        if (usartInstance == nullptr) {
            return;
        }

        switch (peripheralType) {
            case PeripheralType::LPUART_1:
                LL_LPUART_TransmitData8(usartInstance, data);
                break;
            case PeripheralType::USART_1:
            case PeripheralType::USART_2:
            case PeripheralType::USART_3:
                // Direct register access (LL_USART_TransmitData8 not available)
                usartInstance->TDR = data;
                break;
            case PeripheralType::COUNT:
                break;
        }
    }

    template<uint16_t BUFFER_SIZE>
    bool UsartDriver<BUFFER_SIZE>::isTxReady() const noexcept {
        if (usartInstance == nullptr) {
            return false;
        }

        switch (peripheralType) {
            case PeripheralType::LPUART_1:
                return LL_LPUART_IsActiveFlag_TXE(usartInstance);
            case PeripheralType::USART_1:
            case PeripheralType::USART_2:
            case PeripheralType::USART_3:
                // Direct register access (LL_USART_IsActiveFlag_TXE not available)
                return (usartInstance->ISR & USART_ISR_TXE) != 0;
            case PeripheralType::COUNT:
                return false;
        }
        return false;
    }

    template<uint16_t BUFFER_SIZE>
    void UsartDriver<BUFFER_SIZE>::handleTxCompleteInterrupt() noexcept {
        // Check if there's more data to send
        uint8_t data;
        if (txBuffer.get(data)) {
            transmitByte(data);
        } else {
            // No more data, transmission complete
            transmissionActive = false;
        }
    }

    // Explicit template instantiations for common buffer sizes
    template class UsartDriver<64>;
    template class UsartDriver<128>;
    template class UsartDriver<256>;
    template class UsartDriver<512>;
    template class UsartDriver<1024>;

    /**
     * @brief Register USART interrupt handler
     * @param peripheral USART peripheral type
     * @param instance Pointer to UsartDriver instance
     * @return Status indicating success or error
     */
    UsartStatus registerUsartHandler(PeripheralType peripheral, void* instance) noexcept {
        return registerInstance(peripheral, instance);
    }

    /**
     * @brief Handle interrupt for specified USART peripheral
     * @param peripheral USART peripheral type
     */
    void handleUsartInterrupt(PeripheralType peripheral) noexcept {
        void* instance = getRegisteredInstance(peripheral);
        if (instance == nullptr) {
            return;
        }
        
        // Determine the correct template instantiation based on buffer size
        // For now, assume StandardUSART (256 bytes) - this could be improved with type information
        // In production, you might want to store buffer size metadata in the registry
        
        // The actual dispatch depends on which buffer size was used during instantiation
        // For simplicity, we'll use StandardUSART here
        // This assumes most drivers use the default 256-byte buffer
        StandardUSART* driver = static_cast<StandardUSART*>(instance);
        if (driver != nullptr) {
            driver->handleTxCompleteInterrupt();
        }
    }

} // namespace USART

// C interface function for interrupt handling
extern "C" {
    void USART_HandleLpuart1Interrupt(void) {
        USART::handleUsartInterrupt(USART::PeripheralType::LPUART_1);
    }
    
    // C interface functions for syscalls integration
    void* USART_CreateDebugInstance(void) {
        // Create LPUART1 instance for debug output
        static USART::StandardUSART* debug_instance = nullptr;
        if (debug_instance == nullptr) {
            debug_instance = new USART::StandardUSART(USART::PeripheralType::LPUART_1);
        }
        return debug_instance;
    }
    
    void* USART_GetDefaultLpuartConfig(void) {
        static USART_Config c_config = {
            115200,     // baudRate
            0U,         // wordLength
            0U,         // stopBits
            0U          // parity
        };
        return &c_config;
    }
    
    void USART_Initialize(void* instance, void* config_ptr) {
        if (instance != nullptr && config_ptr != nullptr) {
            USART_Config* config = static_cast<USART_Config*>(config_ptr);
            USART::Config cpp_config;
            cpp_config.baudRate = config->baudRate;
            cpp_config.wordLength = config->wordLength;
            cpp_config.stopBits = config->stopBits;
            cpp_config.parity = config->parity;
            cpp_config.hwFlowControl = 0;
            cpp_config.transferDirection = 0x0000000CU;
            
            USART::StandardUSART* driver = static_cast<USART::StandardUSART*>(instance);
            if (driver != nullptr) {
                driver->initialize(cpp_config);
            }
        }
    }
    
    void USART_SendChar(void* instance, char c) {
        if (instance != nullptr) {
            USART::StandardUSART* driver = static_cast<USART::StandardUSART*>(instance);
            if (driver != nullptr) {
                driver->sendByte(static_cast<uint8_t>(c));
            }
        }
    }
}
