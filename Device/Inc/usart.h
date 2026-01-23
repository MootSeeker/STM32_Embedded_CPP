/**
 * @file    usart.h
 * @brief   Intelligent USART class with queue functionality for STM32L4xx
 * @date    2024-06-10
 * @author  MootSeeker
 */

#ifndef INC_USART_H_
#define INC_USART_H_

#include "main.h"
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <cstdint>

// C interface for interrupt handlers
#ifdef __cplusplus
extern "C" {
#endif
    void USART_HandleLpuart1Interrupt(void);
#ifdef __cplusplus
}
#endif

/**
 * @namespace USART
 * @brief Namespace for USART peripheral functions and definitions.
 */
namespace USART 
{
    /**
     * @brief USART peripheral type enumeration
     */
    enum class PeripheralType {
        USART_1,
        USART_2, 
        USART_3,
        LPUART_1
    };

    /**
     * @brief USART configuration structure
     */
    struct Config {
        uint32_t baudRate;
        uint32_t wordLength;
        uint32_t stopBits;
        uint32_t parity;
        uint32_t hwFlowControl;
        uint32_t transferDirection;
    };

    /**
     * @brief Circular buffer for USART data queuing
     * @tparam SIZE Buffer size (must be power of 2 for efficiency)
     */
    template<uint16_t SIZE = 256>
    class CircularBuffer {
        static_assert((SIZE & (SIZE - 1)) == 0, "Buffer size must be power of 2");
        
    private:
        uint8_t buffer[SIZE];
        volatile uint16_t head = 0;
        volatile uint16_t tail = 0;
        static constexpr uint16_t MASK = SIZE - 1;

    public:
        /**
         * @brief Put data into buffer
         * @param data Data to put
         * @return true if successful, false if buffer full
         */
        bool put(uint8_t data) {
            uint16_t next_head = (head + 1) & MASK;
            if (next_head == tail) {
                return false; // Buffer full
            }
            buffer[head] = data;
            head = next_head;
            return true;
        }

        /**
         * @brief Get data from buffer
         * @param data Reference to store retrieved data
         * @return true if successful, false if buffer empty
         */
        bool get(uint8_t& data) {
            if (head == tail) {
                return false; // Buffer empty
            }
            data = buffer[tail];
            tail = (tail + 1) & MASK;
            return true;
        }

        /**
         * @brief Check if buffer is empty
         */
        bool isEmpty() const {
            return head == tail;
        }

        /**
         * @brief Check if buffer is full
         */
        bool isFull() const {
            return ((head + 1) & MASK) == tail;
        }

        /**
         * @brief Get available space in buffer
         */
        uint16_t availableSpace() const {
            return (tail - head - 1) & MASK;
        }

        /**
         * @brief Get number of elements in buffer
         */
        uint16_t size() const {
            return (head - tail) & MASK;
        }

        /**
         * @brief Clear buffer
         */
        void clear() {
            head = 0;
            tail = 0;
        }
    };

    /**
     * @brief USART class with queue functionality
     * @tparam BUFFER_SIZE Size of transmission buffer
     */
    template<uint16_t BUFFER_SIZE = 256>
    class UsartDriver {
    private:
        PeripheralType peripheralType;
        void* usartInstance; // Will point to USART_TypeDef* or USART_TypeDef* 
        Config config;
        CircularBuffer<BUFFER_SIZE> txBuffer;
        volatile bool transmissionActive;
        
        // Private methods for hardware abstraction
        void initializeLpuart();
        void initializeUsart();
        void enableTxInterrupt();
        void disableTxInterrupt();
        void transmitByte(uint8_t data);
        bool isTxReady();
        
    public:
        /**
         * @brief Constructor
         * @param peripheral USART peripheral type
         */
        explicit UsartDriver(PeripheralType peripheral);

        /**
         * @brief Initialize USART with configuration
         * @param cfg Configuration structure
         * @return true if successful
         */
        bool initialize(const Config& cfg);

        /**
         * @brief Send single byte (non-blocking)
         * @param data Byte to send
         * @return true if added to queue successfully
         */
        bool sendByte(uint8_t data);

        /**
         * @brief Send data buffer (non-blocking)
         * @param data Pointer to data
         * @param length Number of bytes to send
         * @return Number of bytes actually queued
         */
        uint16_t sendData(const uint8_t* data, uint16_t length);

        /**
         * @brief Send C-string (non-blocking)
         * @param str Null-terminated string
         * @return Number of bytes actually queued
         */
        uint16_t sendString(const char* str);

        /**
         * @brief Send formatted string (printf-style, non-blocking)
         * @param format Format string
         * @param ... Arguments
         * @return Number of bytes actually queued
         */
        uint16_t sendFormatted(const char* format, ...);

        /**
         * @brief Send hex representation of data
         * @param data Pointer to data
         * @param length Number of bytes
         * @param uppercase Use uppercase hex digits
         * @return Number of bytes actually queued
         */
        uint16_t sendHex(const uint8_t* data, uint16_t length, bool uppercase = true);

        /**
         * @brief Send binary representation of data
         * @param data Pointer to data
         * @param length Number of bytes
         * @return Number of bytes actually queued
         */
        uint16_t sendBinary(const uint8_t* data, uint16_t length);

        /**
         * @brief Check if transmission is active
         */
        bool isTransmissionActive() const {
            return transmissionActive;
        }

        /**
         * @brief Get available buffer space
         */
        uint16_t getAvailableSpace() const {
            return txBuffer.availableSpace();
        }

        /**
         * @brief Get number of bytes in queue
         */
        uint16_t getQueueSize() const {
            return txBuffer.size();
        }

        /**
         * @brief Clear transmission buffer
         */
        void clearBuffer() {
            txBuffer.clear();
        }

        /**
         * @brief Start transmission (called internally and by interrupt)
         */
        void startTransmission();

        /**
         * @brief Handle transmission complete interrupt
         */
        void handleTxCompleteInterrupt();

        /**
         * @brief Get peripheral type
         */
        PeripheralType getPeripheralType() const {
            return peripheralType;
        }

        /**
         * @brief Get USART instance pointer (for interrupt handlers)
         */
        void* getInstance() const {
            return usartInstance;
        }
    };

    // Type aliases for common buffer sizes
    using SmallUSART = UsartDriver<64>;
    using StandardUSART = UsartDriver<256>;
    using LargeUSART = UsartDriver<512>;

    /**
     * @brief Get default configuration for LPUART1
     */
    Config getDefaultLpuartConfig();

    /**
     * @brief Get default configuration for USART
     */
    Config getDefaultUsartConfig();

    /**
     * @brief Register LPUART1 interrupt handler
     */
    void registerLpuart1Handler(void* instance);

    /**
     * @brief Handle LPUART1 interrupt
     */
    void handleLpuart1Interrupt();

    // Global interrupt handlers and instance management
    extern "C" void USART_HandleLpuart1Interrupt(void);

} // namespace USART

// C interface for syscalls integration
#ifdef __cplusplus
extern "C" {
#endif

// C-compatible config structure
typedef struct {
    uint32_t baudRate;
    uint32_t wordLength;
    uint32_t stopBits;
    uint32_t parity;
} USART_Config;

// C interface functions
void* USART_CreateDebugInstance(void);
void* USART_GetDefaultLpuartConfig(void);
void USART_Initialize(void* instance, void* config);
void USART_SendChar(void* instance, char c);

#ifdef __cplusplus
}
#endif

#endif /* INC_USART_H_ */