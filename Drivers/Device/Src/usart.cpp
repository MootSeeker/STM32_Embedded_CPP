/**
 * @fnamespace USART
{
    // Global instance pointer for interrupt handling (simplified approach)
    static void* g_lpuart1Instance = nullptr;  usart.cpp
 * @brief   Intelligent USART class implementation for STM32L4xx
 * @date    2024-06-10
 * @author  MootSeeker
 */

#include "usart.h"
#include "stm32l4xx_ll_lpuart.h"

namespace USART
{
    // Global instance pointer for interrupt handling (simplified approach)
    static void* g_lpuart1Instance = nullptr;

    /**
     * @brief Get default configuration for LPUART1
     */
    Config getDefaultLpuartConfig() {
        Config cfg;
        cfg.baudRate = 115200;
        cfg.wordLength = 0x00000000U;  // LL_LPUART_DATAWIDTH_8B
        cfg.stopBits = 0x00000000U;    // LL_LPUART_STOPBITS_1
        cfg.parity = 0x00000000U;      // LL_LPUART_PARITY_NONE
        cfg.hwFlowControl = 0x00000000U; // LL_LPUART_HWCONTROL_NONE
        cfg.transferDirection = 0x0000000CU; // USART_CR1_TE | USART_CR1_RE (0x08 | 0x04)
        return cfg;
    }

    /**
     * @brief Get default configuration for USART (placeholder for when USART LL is added)
     */
    Config getDefaultUsartConfig() {
        Config cfg;
        cfg.baudRate = 115200;
        // Note: These would be USART-specific constants when USART LL driver is available
        cfg.wordLength = 0; // LL_USART_DATAWIDTH_8B equivalent
        cfg.stopBits = 0;   // LL_USART_STOPBITS_1 equivalent  
        cfg.parity = 0;     // LL_USART_PARITY_NONE equivalent
        cfg.hwFlowControl = 0; // LL_USART_HWCONTROL_NONE equivalent
        cfg.transferDirection = 0; // LL_USART_DIRECTION_TX_RX equivalent
        return cfg;
    }

    template<uint16_t BUFFER_SIZE>
    UsartDriver<BUFFER_SIZE>::UsartDriver(PeripheralType peripheral)
        : peripheralType(peripheral), usartInstance(nullptr), transmissionActive(false) {
        
        // Set the hardware instance based on peripheral type
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
        }
    }

    template<uint16_t BUFFER_SIZE>
    bool UsartDriver<BUFFER_SIZE>::initialize(const Config& cfg) {
        config = cfg;
        
        switch (peripheralType) {
            case PeripheralType::LPUART_1:
                initializeLpuart();
                break;
            case PeripheralType::USART_1:
            case PeripheralType::USART_2:
            case PeripheralType::USART_3:
                initializeUsart();
                break;
        }
        
        return true;
    }

    template<uint16_t BUFFER_SIZE>
    void UsartDriver<BUFFER_SIZE>::initializeLpuart() {
        USART_TypeDef* lpuart = static_cast<USART_TypeDef*>(usartInstance);
        
        // Enable LPUART1 clock
        LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPUART1);
        
        // Configure LPUART
        LL_LPUART_SetBaudRate(lpuart, LL_RCC_GetLPUARTClockFreq(LL_RCC_LPUART1_CLKSOURCE), config.baudRate);
        LL_LPUART_SetDataWidth(lpuart, config.wordLength);
        LL_LPUART_SetStopBitsLength(lpuart, config.stopBits);
        LL_LPUART_SetParity(lpuart, config.parity);
        LL_LPUART_SetTransferDirection(lpuart, config.transferDirection);
        LL_LPUART_SetHWFlowCtrl(lpuart, config.hwFlowControl);
        
        // Enable LPUART
        LL_LPUART_Enable(lpuart);
        
        // Register this instance for interrupt handling
        registerLpuart1Handler(this);
        
        // Enable TX empty interrupt
        LL_LPUART_EnableIT_TXE(lpuart);
        
        // Enable NVIC interrupt
        NVIC_SetPriority(LPUART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(LPUART1_IRQn);
    }

    template<uint16_t BUFFER_SIZE>
    void UsartDriver<BUFFER_SIZE>::initializeUsart() {
        // Note: This would be implemented when USART LL drivers are available
        // For now, this is a placeholder that shows the structure
        [[maybe_unused]] USART_TypeDef* usart = static_cast<USART_TypeDef*>(usartInstance);
        
        // Enable appropriate clock based on USART instance
        switch (peripheralType) {
            case PeripheralType::USART_1:
                LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
                break;
            case PeripheralType::USART_2:
                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
                break;
            case PeripheralType::USART_3:
                LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
                break;
            default:
                break;
        }
        
        // TODO: Implement USART configuration when LL drivers are available
        // This would include baud rate, data width, stop bits, parity, etc.
    }

    template<uint16_t BUFFER_SIZE>
    bool UsartDriver<BUFFER_SIZE>::sendByte(uint8_t data) {
        bool success = txBuffer.put(data);
        if (success && !transmissionActive) {
            startTransmission();
        }
        return success;
    }

    template<uint16_t BUFFER_SIZE>
    uint16_t UsartDriver<BUFFER_SIZE>::sendData(const uint8_t* data, uint16_t length) {
        if (data == nullptr) return 0;
        
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
    uint16_t UsartDriver<BUFFER_SIZE>::sendString(const char* str) {
        if (str == nullptr) return 0;
        
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
    uint16_t UsartDriver<BUFFER_SIZE>::sendFormatted(const char* format, ...) {
        if (format == nullptr) return 0;
        
        char buffer[256]; // Temporary buffer for formatted string
        va_list args;
        va_start(args, format);
        int length = vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        
        if (length > 0) {
            return sendData(reinterpret_cast<const uint8_t*>(buffer), 
                           static_cast<uint16_t>(length));
        }
        
        return 0;
    }

    template<uint16_t BUFFER_SIZE>
    uint16_t UsartDriver<BUFFER_SIZE>::sendHex(const uint8_t* data, uint16_t length, bool uppercase) {
        if (data == nullptr) return 0;
        
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
    uint16_t UsartDriver<BUFFER_SIZE>::sendBinary(const uint8_t* data, uint16_t length) {
        if (data == nullptr) return 0;
        
        uint16_t sent = 0;
        
        for (uint16_t i = 0; i < length; i++) {
            uint8_t byte = data[i];
            
            // Send each bit (MSB first)
            for (int bit = 7; bit >= 0; bit--) {
                char bitChar = ((byte >> bit) & 0x01) ? '1' : '0';
                if (!txBuffer.put(bitChar)) {
                    goto exit_loops;
                }
                sent++;
            }
        }
        
    exit_loops:
        if (sent > 0 && !transmissionActive) {
            startTransmission();
        }
        
        return sent;
    }

    template<uint16_t BUFFER_SIZE>
    void UsartDriver<BUFFER_SIZE>::startTransmission() {
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
    void UsartDriver<BUFFER_SIZE>::transmitByte(uint8_t data) {
        switch (peripheralType) {
            case PeripheralType::LPUART_1: {
                USART_TypeDef* lpuart = static_cast<USART_TypeDef*>(usartInstance);
                LL_LPUART_TransmitData8(lpuart, data);
                break;
            }
            case PeripheralType::USART_1:
            case PeripheralType::USART_2:
            case PeripheralType::USART_3: {
                // TODO: Implement when USART LL drivers are available
                [[maybe_unused]] USART_TypeDef* usart = static_cast<USART_TypeDef*>(usartInstance);
                // usart->TDR = data; // Direct register access as fallback
                break;
            }
        }
    }

    template<uint16_t BUFFER_SIZE>
    bool UsartDriver<BUFFER_SIZE>::isTxReady() {
        switch (peripheralType) {
            case PeripheralType::LPUART_1: {
                USART_TypeDef* lpuart = static_cast<USART_TypeDef*>(usartInstance);
                return LL_LPUART_IsActiveFlag_TXE(lpuart);
            }
            case PeripheralType::USART_1:
            case PeripheralType::USART_2:
            case PeripheralType::USART_3: {
                USART_TypeDef* usart = static_cast<USART_TypeDef*>(usartInstance);
                return (usart->ISR & USART_ISR_TXE) != 0;
            }
        }
        return false;
    }

    template<uint16_t BUFFER_SIZE>
    void UsartDriver<BUFFER_SIZE>::handleTxCompleteInterrupt() {
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

    // Global interrupt handler functions
    void registerLpuart1Handler(void* instance) {
        g_lpuart1Instance = instance;
    }

    void handleLpuart1Interrupt() {
        // Handle LPUART1 TXE interrupt
        if (g_lpuart1Instance != nullptr) {
            // For now, just cast and call the interrupt handler
            // In production, you'd want to check the actual hardware flags
            static_cast<UsartDriver<256>*>(g_lpuart1Instance)->handleTxCompleteInterrupt();
        }
    }

} // namespace USART

// C interface function for interrupt handling
extern "C" {
    void USART_HandleLpuart1Interrupt(void) {
        USART::handleLpuart1Interrupt();
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
        USART::Config cpp_config = USART::getDefaultLpuartConfig();
        static USART_Config c_config;
        c_config.baudRate = cpp_config.baudRate;
        c_config.wordLength = cpp_config.wordLength;
        c_config.stopBits = cpp_config.stopBits;
        c_config.parity = cpp_config.parity;
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
            
            static_cast<USART::StandardUSART*>(instance)->initialize(cpp_config);
        }
    }
    
    void USART_SendChar(void* instance, char c) {
        if (instance != nullptr) {
            static_cast<USART::StandardUSART*>(instance)->sendByte(static_cast<uint8_t>(c));
        }
    }
}
