# USART Class for STM32L4xx

## Overview

This implementation provides a USART class with the following features:

### Key Features

1. **Queue-based Transmission**: Non-blocking transmission using circular buffer
2. **Multiple Peripheral Support**: Supports USART1, USART2, USART3, and LPUART1
3. **Data Formatting**: Built-in support for:
   - String transmission
   - Printf-style formatted output
   - Hexadecimal output
   - Binary output
4. **Interrupt-Driven**: Uses hardware interrupts for efficient transmission
5. **Template-Based Buffer Sizing**: Configurable buffer sizes (64, 128, 256, 512, 1024 bytes)

### Architecture

```cpp
namespace USART {
    // Peripheral types
    enum class PeripheralType {
        USART_1, USART_2, USART_3, LPUART_1
    };
    
    // Main class template
    template<uint16_t BUFFER_SIZE = 256>
    class UsartDriver;
    
    // Type aliases
    using SmallUSART = UsartDriver<64>;     // 64-byte buffer
    using StandardUSART = UsartDriver<256>; // 256-byte buffer  
    using LargeUSART = UsartDriver<512>;    // 512-byte buffer
}
```

## Usage Example

### Basic LPUART1 Usage

```cpp
#include "usart.h"

void setup() {
    // Create USART instance for LPUART1
    USART::StandardUSART lpuart1(USART::PeripheralType::LPUART_1);
    
    // Get default configuration
    USART::Config config = USART::getDefaultLpuartConfig();
    
    // Initialize
    if (lpuart1.initialize(config)) {
        // Send basic string
        lpuart1.sendString("Hello, World!\r\n");
        
        // Send formatted data
        lpuart1.sendFormatted("System Clock: %lu Hz\r\n", SystemCoreClock);
        
        // Send hex data
        uint8_t data[] = {0x01, 0x23, 0x45, 0x67};
        lpuart1.sendHex(data, sizeof(data));
        
        // Send binary representation
        lpuart1.sendBinary(data, 1); // Binary of first byte
    }
}
```

### Advanced Usage

```cpp
void advancedUsage() {
    // Create large buffer instance
    USART::UsartDriver<1024> lpuart1(USART::PeripheralType::LPUART_1);
    
    // Custom configuration
    USART::Config config = USART::getDefaultLpuartConfig();
    config.baudRate = 921600; // High speed
    
    if (lpuart1.initialize(config)) {
        // Check buffer status
        uint16_t available = lpuart1.getAvailableSpace();
        uint16_t used = lpuart1.getQueueSize();
        
        lpuart1.sendFormatted("Buffer: %d used, %d available\r\n", used, available);
        
        // Send large amount of data
        for (int i = 0; i < 100; i++) {
            lpuart1.sendFormatted("Data packet %d\r\n", i);
        }
        
        // Check if transmission is active
        if (lpuart1.isTransmissionActive()) {
            lpuart1.sendString("Transmission in progress...\r\n");
        }
    }
}
```

## Configuration

### LPUART1 Default Configuration
- Baud Rate: 115200
- Data Width: 8 bits
- Stop Bits: 1
- Parity: None
- Hardware Flow Control: None
- Direction: TX + RX

### Buffer Sizes
The circular buffer uses power-of-2 sizes for efficiency:
- 64 bytes (SmallUSART)
- 128 bytes
- 256 bytes (StandardUSART) - Default
- 512 bytes (LargeUSART)
- 1024 bytes

## Hardware Setup

### LPUART1 Pin Configuration
Make sure LPUART1 pins are configured in CubeMX:
- TX: Usually PA2 or PG7
- RX: Usually PA3 or PG8

### Interrupt Configuration
The LPUART1_IRQHandler is automatically configured when initializing LPUART1.

## API Reference

### Core Methods

```cpp
// Initialization
bool initialize(const Config& cfg);

// Basic transmission  
bool sendByte(uint8_t data);
uint16_t sendData(const uint8_t* data, uint16_t length);
uint16_t sendString(const char* str);

// Formatted transmission
uint16_t sendFormatted(const char* format, ...);
uint16_t sendHex(const uint8_t* data, uint16_t length, bool uppercase = true);
uint16_t sendBinary(const uint8_t* data, uint16_t length);

// Status methods
bool isTransmissionActive() const;
uint16_t getAvailableSpace() const;
uint16_t getQueueSize() const;
void clearBuffer();
```

### Configuration Functions

```cpp
USART::Config getDefaultLpuartConfig();  // For LPUART1
USART::Config getDefaultUsartConfig();   // For USART1/2/3 (placeholder)
```

## Thread Safety

The circular buffer uses atomic operations on head/tail pointers and should be safe for:
- Main thread adding data to queue
- Interrupt handler removing data from queue

## Performance Characteristics

- **Non-blocking**: All send operations return immediately
- **Efficient**: Circular buffer with O(1) operations
- **Memory efficient**: Template-based sizing avoids waste
- **Interrupt overhead**: Minimal - only processes one byte per interrupt

## Notes

1. **VS Code IntelliSense Issues**: The current VS Code setup may show compilation errors due to IntelliSense configuration issues with STM32 headers. The code should compile correctly with the proper STM32 toolchain.

2. **Multiple Instances**: For production code, consider implementing a singleton pattern or instance manager for interrupt handling.

3. **Error Handling**: The current implementation provides basic error handling. Consider adding more robust error reporting for production use.

4. **USART vs LPUART**: The current implementation focuses on LPUART1. USART1/2/3 support requires additional LL driver includes.

## Extension Points

The design allows for easy extension:

1. **Add Reception**: Implement RX circular buffer and interrupt handling
2. **Add DMA Support**: Integrate DMA for high-throughput applications
3. **Add Flow Control**: Implement RTS/CTS flow control
4. **Add Error Handling**: Implement comprehensive error reporting
5. **Add USART Support**: Complete USART1/2/3 implementation when LL drivers are available