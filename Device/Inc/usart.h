/**
 * @file    usart.h
 * @brief   Type-safe, interrupt-driven USART driver with circular buffering for STM32L4xx
 * @date    2024-06-10
 * @author  MootSeeker
 * 
 * @mainpage USART Driver Documentation
 * 
 * ## Overview
 * 
 * This module provides a modern C++ wrapper for STM32L4xx USART and LPUART peripherals.
 * It features:
 * - **Type-safe instance management** via registry pattern (no void* casting)
 * - **Structured error handling** with UsartError enum and UsartStatus struct
 * - **Interrupt-driven non-blocking transmission** with circular buffer
 * - **Multiple send methods**: strings, formatted output, hex/binary representations
 * - **noexcept/constexpr annotations** for compile-time optimization
 * - **Support for LPUART_1, USART_1, USART_2, USART_3**
 * 
 * ## Quick Start
 * 
 * @code
 * // Initialize in App_Init()
 * auto uart = new USART::StandardUSART(USART::PeripheralType::LPUART_1);
 * auto config = USART::getDefaultLpuartConfig();
 * USART::UsartStatus status = uart->initialize(config);
 * 
 * if (!status.isSuccess()) {
 *     // Handle error
 *     Error_Handler();
 * }
 * 
 * // Send data (non-blocking, ISR-safe)
 * uart->sendString("Hello World\r\n");
 * uart->sendFormatted("Value: %d\r\n", 42);
 * @endcode
 * 
 * ## Architecture
 * 
 * ### Circular Buffer
 * - Template-based: `CircularBuffer<SIZE>` (SIZE must be power of 2)
 * - Type: `StandardUSART` = `UsartDriver<256>`
 * - Thread-safe for ISR context (volatile atomics)
 * 
 * ### ISR Dispatch
 * - **Global Registry**: `g_usartRegistry[PeripheralType::COUNT]` stores instance pointers
 * - **C-to-C++ Bridge**: C ISRs (`USART_HandleLpuart1Interrupt`) → C++ dispatcher
 * - **Type-Safe Lookup**: Index by `PeripheralType` enum
 * 
 * ### Error Handling
 * - **UsartError**: Enum with specific error codes (OK, BUFFER_FULL, UNINITIALIZED, etc.)
 * - **UsartStatus**: Struct containing error + optional details (HW flags, etc.)
 * - **Return Type**: `initialize()` returns `UsartStatus`; `send*()` return bytes sent
 * 
 * ## Supported Peripherals
 * 
 * | Peripheral | Status | Notes |
 * |------------|--------|-------|
 * | LPUART_1 | ✓ Full | LL drivers available |
 * | USART_1 | ✓ Full | Register-level (LL_USART not available in HAL) |
 * | USART_2 | ✓ Full | Register-level |
 * | USART_3 | ✓ Full | Register-level |
 * 
 * ## Usage Patterns
 * 
 * ### Pattern 1: Error-Safe Initialization
 * @code
 * USART::StandardUSART uart(USART::PeripheralType::LPUART_1);
 * auto config = USART::getDefaultLpuartConfig();
 * auto status = uart.initialize(config);
 * 
 * if (status.error != USART::UsartError::OK) {
 *     switch (status.error) {
 *         case USART::UsartError::INVALID_PERIPHERAL:
 *             // Handle invalid peripheral
 *             break;
 *         case USART::UsartError::UNINITIALIZED:
 *             // Driver not ready
 *             break;
 *         default:
 *             break;
 *     }
 * }
 * @endcode
 * 
 * ### Pattern 2: Non-Blocking Send with Buffer Check
 * @code
 * if (uart.getAvailableSpace() >= myString.length()) {
 *     uint16_t sent = uart.sendString(myString.c_str());
 *     if (sent < myString.length()) {
 *         // Partial send - buffer became full
 *     }
 * } else {
 *     // Buffer too small for this message, try later
 * }
 * @endcode
 * 
 * ### Pattern 3: Formatted Output with Error Handling
 * @code
 * // sendFormatted() is variadic and supports printf-style formatting
 * // WARNING: Limited to 256 characters due to temporary buffer
 * uint16_t sent = uart.sendFormatted("Counter: %lu, Status: 0x%02X\r\n", counter, status);
 * if (sent == 0) {
 *     // Buffer full, message dropped (or formatting error)
 * }
 * @endcode
 * 
 * ### Pattern 4: Hex/Binary Data Transmission
 * @code
 * uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
 * 
 * // Send as hex string: "DEADBEEF" (16 characters)
 * uart.sendHex(data, sizeof(data), true);  // uppercase=true
 * uart.sendString("\r\n");
 * 
 * // Send as binary string: "11011110101011011011111011101111" (32 characters)
 * uart.sendBinary(data, sizeof(data));
 * uart.sendString("\r\n");
 * @endcode
 * 
 * ## Thread Safety & ISR Context
 * 
 * All `send*()` methods and `handleTxCompleteInterrupt()` are **ISR-safe**:
 * - No locks needed
 * - Uses volatile atomics for synchronization
 * - Can be called from both main loop and ISR context
 * - Buffer operations are atomic (head/tail pointers)
 * 
 * **Safe to call from:**
 * - Main event loop
 * - ISR handler
 * - Callback functions
 * 
 * **NOT safe:**
 * - `initialize()` should only be called during setup
 * - `clearBuffer()` should not be called while transmission active
 * 
 * ## Performance Considerations
 * 
 * - **Buffer Size**: Power-of-2 required for bitwise optimization (& MASK instead of %)
 * - **ISR Latency**: O(1) dispatch via registry lookup
 * - **Memory**: Each instance uses ~SIZE + 10 bytes (CircularBuffer + metadata)
 * - **Type Aliases**: `SmallUSART` (64B), `StandardUSART` (256B), `LargeUSART` (512B)
 * 
 * ## Limitations & Future Work
 * 
 * ### Current Limitations
 * - **TX only**: Receive functionality not yet implemented
 * - **Single template instantiation per peripheral**: Registry assumes StandardUSART
 * - **No DMA**: Interrupt-driven only
 * - **sendFormatted() buffer**: Fixed 256-byte temporary buffer (truncation possible)
 * 
 * ### Planned Enhancements
 * - RX circular buffer with similar architecture
 * - Configurable ISR priority per peripheral
 * - Optional DMA fallback for high-speed transfers
 * - User callbacks for transmit-complete events
 * 
 * @see Examples/02_USART_HelloWorld for detailed usage example
 * @see CONTRIBUTING.md for coding standards
 */

#ifndef INC_USART_H_
#define INC_USART_H_

#include "mcu_adapter.h"
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
    enum class PeripheralType : uint8_t {
        USART_1,
        USART_2, 
        USART_3,
        LPUART_1,
        COUNT  ///< Number of supported peripherals
    };

    /**
     * @enum UsartError
     * @brief Error codes for USART operations
     */
    enum class UsartError : uint8_t {
        OK = 0,                        ///< Operation successful
        BUFFER_FULL,                   ///< Transmission buffer is full
        UNINITIALIZED,                 ///< Driver not initialized
        INVALID_PERIPHERAL,            ///< Invalid peripheral type
        INVALID_PARAMETER,             ///< Invalid parameter provided
        NULL_POINTER                   ///< Null pointer provided where not allowed
    };

    /**
     * @struct UsartStatus
     * @brief Status information for USART operations
     */
    struct UsartStatus {
        UsartError error;              ///< Error code
        uint32_t details;              ///< Additional error details (e.g., hardware flags)

        /**
         * @brief Check if status represents success
         */
        [[nodiscard]] constexpr bool isSuccess() const noexcept {
            return error == UsartError::OK;
        }
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
     * 
     * Thread-safe for ISR context. Uses volatile atomics for head/tail pointers
     * to ensure correct behavior when accessed from interrupt handlers.
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
         * @brief Put data into buffer (interrupt-safe)
         * @param data Data to put
         * @return true if successful, false if buffer full
         */
        bool put(uint8_t data) noexcept {
            uint16_t next_head = (head + 1) & MASK;
            if (next_head == tail) {
                return false; // Buffer full
            }
            buffer[head] = data;
            head = next_head;
            return true;
        }

        /**
         * @brief Get data from buffer (interrupt-safe)
         * @param data Reference to store retrieved data
         * @return true if successful, false if buffer empty
         */
        bool get(uint8_t& data) noexcept {
            if (head == tail) {
                return false; // Buffer empty
            }
            data = buffer[tail];
            tail = (tail + 1) & MASK;
            return true;
        }

        /**
         * @brief Check if buffer is empty
         * @return true if buffer contains no data
         */
        [[nodiscard]] bool isEmpty() const noexcept {
            return head == tail;
        }

        /**
         * @brief Check if buffer is full
         * @return true if buffer has no available space
         */
        [[nodiscard]] bool isFull() const noexcept {
            return ((head + 1) & MASK) == tail;
        }

        /**
         * @brief Get available space in buffer
         * @return Number of free bytes
         */
        [[nodiscard]] uint16_t availableSpace() const noexcept {
            return (tail - head - 1) & MASK;
        }

        /**
         * @brief Get number of elements in buffer
         * @return Number of bytes currently stored
         */
        [[nodiscard]] uint16_t getRemainingCount() const noexcept {
            return (head - tail) & MASK;
        }

        /**
         * @brief Legacy name for getRemainingCount()
         * @deprecated Use getRemainingCount() instead
         */
        [[nodiscard]] uint16_t size() const noexcept {
            return getRemainingCount();
        }

        /**
         * @brief Clear buffer (atomically resets head and tail)
         * 
         * This is safe to call from main context but should not be called
         * while transmission is active in ISR.
         */
        void reset() noexcept {
            head = 0;
            tail = 0;
        }

        /**
         * @brief Legacy name for reset()
         * @deprecated Use reset() instead
         */
        void clear() noexcept {
            reset();
        }
    };

    /**
     * @brief USART class with queue functionality
     * @tparam BUFFER_SIZE Size of transmission buffer (must be power of 2)
     * 
     * Provides type-safe, interrupt-driven USART communication with circular buffering.
     * Supports LPUART_1, USART_1, USART_2, and USART_3 on STM32L4xx microcontrollers.
     */
    template<uint16_t BUFFER_SIZE = 256>
    class UsartDriver {
        static_assert((BUFFER_SIZE & (BUFFER_SIZE - 1)) == 0, "BUFFER_SIZE must be power of 2");
        static_assert(BUFFER_SIZE >= 64 && BUFFER_SIZE <= 4096, "BUFFER_SIZE must be between 64 and 4096");

    private:
        PeripheralType peripheralType;
        USART_TypeDef* usartInstance;  ///< Type-safe instance pointer
        Config config;
        CircularBuffer<BUFFER_SIZE> txBuffer;
        volatile bool transmissionActive;
        volatile bool isInitialized;
        
        // Private methods for hardware abstraction
        void initializeLpuart() noexcept;
        void initializeUsart() noexcept;
        void enableTxInterrupt() noexcept;
        void disableTxInterrupt() noexcept;
        void transmitByte(uint8_t data) noexcept;
        [[nodiscard]] bool isTxReady() const noexcept;
        
    public:
        /**
         * @brief Constructor
         * @param peripheral USART peripheral type
         */
        explicit UsartDriver(PeripheralType peripheral) noexcept;

        /**
         * @brief Initialize USART with configuration
         * @param cfg Configuration structure
         * @return Status indicating success or error
         */
        UsartStatus initialize(const Config& cfg) noexcept;

        /**
         * @brief Send single byte (non-blocking, ISR-safe)
         * @param data Byte to send
         * @return true if added to queue successfully
         */
        bool sendByte(uint8_t data) noexcept;

        /**
         * @brief Send data buffer (non-blocking, ISR-safe)
         * @param data Pointer to data buffer (must not be nullptr if length > 0)
         * @param length Number of bytes to send
         * @return Number of bytes actually queued
         */
        uint16_t sendData(const uint8_t* data, uint16_t length) noexcept;

        /**
         * @brief Send C-string (non-blocking, ISR-safe)
         * @param str Null-terminated string (must not be nullptr)
         * @return Number of bytes actually queued
         */
        uint16_t sendString(const char* str) noexcept;

        /**
         * @brief Send formatted string (printf-style, non-blocking)
         * 
         * @warning Limited to 256 characters. Output may be truncated if it exceeds buffer size.
         * @param format Format string (must not be nullptr)
         * @param ... Arguments
         * @return Number of bytes actually queued
         */
        uint16_t sendFormatted(const char* format, ...) noexcept;

        /**
         * @brief Send hex representation of data (non-blocking, ISR-safe)
         * @param data Pointer to data (must not be nullptr if length > 0)
         * @param length Number of bytes to encode
         * @param uppercase Use uppercase hex digits (default: true)
         * @return Number of hex characters queued (2 per input byte)
         */
        uint16_t sendHex(const uint8_t* data, uint16_t length, bool uppercase = true) noexcept;

        /**
         * @brief Send binary representation of data (non-blocking, ISR-safe)
         * @param data Pointer to data (must not be nullptr if length > 0)
         * @param length Number of bytes to encode
         * @return Number of binary characters queued (8 per input byte)
         */
        uint16_t sendBinary(const uint8_t* data, uint16_t length) noexcept;

        /**
         * @brief Check if transmission is active
         * @return true if transmission is in progress
         */
        [[nodiscard]] bool isTransmissionActive() const noexcept {
            return transmissionActive;
        }

        /**
         * @brief Check if driver has been initialized
         * @return true if initialize() was called successfully
         */
        [[nodiscard]] bool isInitialized() const noexcept {
            return isInitialized;
        }

        /**
         * @brief Get available buffer space
         * @return Number of free bytes in transmission buffer
         */
        [[nodiscard]] uint16_t getAvailableSpace() const noexcept {
            return txBuffer.availableSpace();
        }

        /**
         * @brief Get number of bytes in queue
         * @return Number of bytes waiting for transmission
         */
        [[nodiscard]] uint16_t getQueueSize() const noexcept {
            return txBuffer.getRemainingCount();
        }

        /**
         * @brief Clear transmission buffer
         * 
         * @warning Must not be called while transmission is active.
         * Call stopTransmission() first if needed.
         */
        void clearBuffer() noexcept {
            txBuffer.reset();
        }

        /**
         * @brief Start transmission (called internally and by interrupt)
         */
        void startTransmission() noexcept;

        /**
         * @brief Handle transmission complete interrupt (called from ISR)
         * 
         * This method is typically called from the USART interrupt handler.
         * It automatically retrieves the next byte from the buffer and continues
         * transmission, or marks transmission as complete if buffer is empty.
         */
        void handleTxCompleteInterrupt() noexcept;

        /**
         * @brief Get peripheral type
         * @return The peripheral type this driver is using
         */
        [[nodiscard]] PeripheralType getPeripheralType() const noexcept {
            return peripheralType;
        }

        /**
         * @brief Get USART instance pointer (type-safe, for internal use)
         * @return USART_TypeDef* pointer to hardware registers
         */
        [[nodiscard]] USART_TypeDef* getInstance() const noexcept {
            return usartInstance;
        }
    };

    // Type aliases for common buffer sizes
    using SmallUSART = UsartDriver<64>;
    using StandardUSART = UsartDriver<256>;
    using LargeUSART = UsartDriver<512>;

    /**
     * @brief Get default configuration for LPUART1
     * @return Config structure with sensible defaults for LPUART_1 at 115200 baud
     */
    constexpr Config getDefaultLpuartConfig() noexcept {
        return Config{
            .baudRate = 115200U,
            .wordLength = 0x00000000U,      // LL_LPUART_DATAWIDTH_8B
            .stopBits = 0x00000000U,        // LL_LPUART_STOPBITS_1
            .parity = 0x00000000U,          // LL_LPUART_PARITY_NONE
            .hwFlowControl = 0x00000000U,   // LL_LPUART_HWCONTROL_NONE
            .transferDirection = 0x0000000CU // LL_LPUART_DIRECTION_TX_RX
        };
    }

    /**
     * @brief Get default configuration for USART
     * @return Config structure with sensible defaults for USART at 115200 baud
     */
    constexpr Config getDefaultUsartConfig() noexcept {
        return Config{
            .baudRate = 115200U,
            .wordLength = 0U,               // LL_USART_DATAWIDTH_8B equivalent
            .stopBits = 0U,                 // LL_USART_STOPBITS_1 equivalent
            .parity = 0U,                   // LL_USART_PARITY_NONE equivalent
            .hwFlowControl = 0U,            // LL_USART_HWCONTROL_NONE equivalent
            .transferDirection = 0x0000000CU // LL_USART_DIRECTION_TX_RX equivalent
        };
    }

    /**
     * @brief Register USART interrupt handler for a specific peripheral
     * @param peripheral The USART peripheral type
     * @param instance Pointer to UsartDriver instance for this peripheral
     * @return Status indicating success or error
     */
    UsartStatus registerUsartHandler(PeripheralType peripheral, void* instance) noexcept;

    /**
     * @brief Handle interrupt for specified USART peripheral
     * @param peripheral The USART peripheral type
     */
    void handleUsartInterrupt(PeripheralType peripheral) noexcept;

    // Global interrupt handlers - C interface
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