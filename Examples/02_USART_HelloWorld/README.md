# USART Hello World Example

Demonstrates the refactored USART C++ driver with modern error handling, type safety, and non-blocking circular buffering.

## Overview

This example showcases the STM32L433 LPUART1 interface at 115200 baud, sending various formatted messages to demonstrate the driver's capabilities:

- **Simple text** transmission
- **Formatted strings** (printf-style)
- **Hex representation** of binary data
- **Binary representation** of bytes
- **Transmission status** tracking
- **Buffer management** and error checking

## Hardware Setup

| Component | Pin | Note |
|-----------|-----|------|
| STM32L433 Nucleo | PA2, PA3 | LPUART1 (routed to ST-Link) |
| ST-Link | USB | Connected to debugger/UART bridge |
| Serial Terminal | - | 115200 baud, 8-N-1 |

## Key Concepts

### 1. **Type-Safe Initialization**
```cpp
USART::StandardUSART* debugUart = 
    new USART::StandardUSART(USART::PeripheralType::LPUART_1);

USART::Config config = USART::getDefaultLpuartConfig();
USART::UsartStatus status = debugUart->initialize(config);

if (!status.isSuccess()) {
    // Handle error using UsartError enum
    switch(status.error) {
        case USART::UsartError::INVALID_PERIPHERAL:
            // ...
            break;
        // ... other cases
    }
}
```

### 2. **Non-Blocking Transmission**
All `send*()` methods are non-blocking and ISR-safe:
```cpp
// Return value indicates how many bytes were queued (0 if buffer full)
uint16_t sent = debugUart->sendString("Hello\r\n");

if (sent < strlen("Hello\r\n")) {
    // Buffer was full, some bytes not queued
}
```

### 3. **Circular Buffer Management**
```cpp
// Check available space
uint16_t available = debugUart->getAvailableSpace();

// Get current queue size
uint16_t pending = debugUart->getQueueSize();

// Clear buffer (only safe when transmission not active)
debugUart->clearBuffer();
```

### 4. **Various Send Methods**

#### Simple String Transmission
```cpp
debugUart->sendString("Simple text\r\n");
```

#### Formatted Output (printf-style)
```cpp
debugUart->sendFormatted("Value: %d, Hex: 0x%X\r\n", 42, 0xDEAD);
```

#### Hex Representation
```cpp
uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
debugUart->sendHex(data, sizeof(data), true);  // Uppercase hex
```

#### Binary Representation
```cpp
uint8_t byte = 0b10101010;
debugUart->sendBinary(&byte, 1);  // Output: "10101010"
```

### 5. **Error Handling**

The driver provides structured error reporting:
```cpp
enum class UsartError : uint8_t {
    OK = 0,                    // Success
    BUFFER_FULL,              // Transmission buffer exhausted
    UNINITIALIZED,            // Driver not initialized
    INVALID_PERIPHERAL,       // Invalid peripheral selection
    INVALID_PARAMETER,        // Invalid configuration parameter
    NULL_POINTER              // Null pointer provided
};

struct UsartStatus {
    UsartError error;
    uint32_t details;          // Additional error context
    
    bool isSuccess() const;    // Convenience check
};
```

## Output

Expected serial terminal output:

```
========================================
USART Refactored Driver Example
STM32L433 LPUART1 @ 115200 baud
========================================

--- Message 0 ---
Counter value: 0
Remaining buffer space: 241 bytes
Hex representation: DEADBEEF
Binary (first byte): 11011110
  [Transmission complete]

--- Message 1 ---
Counter value: 1
Remaining buffer space: 241 bytes
Hex representation: DEADBEEF
Binary (first byte): 11011110
  [Transmission complete]

...
```

## Improvements Over Original Driver

| Feature | Original | Refactored |
|---------|----------|-----------|
| Type Safety | `void*` pointers | `USART_TypeDef*` + Registry |
| Error Handling | Boolean returns | `UsartError` + `UsartStatus` |
| Code Quality | Anti-patterns (goto) | Modern C++ (noexcept, constexpr) |
| USART Support | LPUART1 only (stub) | Full LPUART1, USART1-3 |
| Documentation | Basic | Comprehensive Doxygen |
| ISR Safety | Limited | Explicit volatile semantics |

## Future Enhancements

1. **RX Buffer Implementation** - Receive circular buffer for incoming data
2. **Callback Registration** - User callbacks on transmission complete
3. **DMA Support** - High-speed transfers with DMA
4. **Flow Control** - Hardware flow control (RTS/CTS)

## Build & Run

1. Open in STM32CubeIDE
2. Compile the project
3. Flash to STM32L433 Nucleo
4. Open serial terminal at 115200 baud
5. Observe debug messages

## Notes

- The driver uses ISR-safe circular buffering with volatile pointers
- All `send*()` methods are non-blocking and can be called from any context
- `sendFormatted()` uses a 256-byte temporary buffer (see documentation for limits)
- Buffer overflow is handled gracefully (returns bytes actually sent, not dropped silently)
- ISR dispatch is type-safe through registry-based instance lookup

## Related Files

- [Device/Inc/usart.h](../../Device/Inc/usart.h) - Driver header with full documentation
- [Device/Src/usart.cpp](../../Device/Src/usart.cpp) - Implementation
- [CONTRIBUTING.md](../../CONTRIBUTING.md) - Project coding standards
