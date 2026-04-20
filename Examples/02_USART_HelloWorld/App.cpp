/**
 * @file    App.cpp
 * @brief   USART "Hello World" Example - Refactored Driver with Error Handling
 * @date    2024-06-10
 * @author  MootSeeker
 * 
 * Demonstrates the refactored USART driver with:
 * - Type-safe peripheral selection
 * - Error handling with UsartStatus
 * - Non-blocking transmission with circular buffer
 * - String formatting capabilities
 * 
 * Hardware: STM32L433 Nucleo board
 * - LPUART1 connected to ST-Link for debug output
 * - TX: PA2, RX: PA3 (note: LPUART uses different pins than USART)
 */

#include "App.h"
#include "usart.h"

// Global USART driver instance - allocated during initialization
static USART::StandardUSART* g_debugUart = nullptr;

/**
 * @brief Initialize the USART subsystem
 * 
 * Sets up LPUART1 at 115200 baud for debug output.
 * This function is called once at startup before entering the main event loop.
 */
void App_Init(void)
{
    // Create LPUART1 driver instance with standard 256-byte buffer
    g_debugUart = new USART::StandardUSART(USART::PeripheralType::LPUART_1);
    
    if (g_debugUart == nullptr) {
        // Memory allocation failed - in production, call Error_Handler()
        Error_Handler();
        return;
    }
    
    // Get default configuration (115200 baud, 8-N-1)
    USART::Config config = USART::getDefaultLpuartConfig();
    
    // Initialize the driver
    USART::UsartStatus status = g_debugUart->initialize(config);
    
    if (!status.isSuccess()) {
        // Initialization failed - log error if possible
        // In production, call Error_Handler()
        Error_Handler();
        return;
    }
    
    // Send welcome message
    g_debugUart->sendString("========================================\r\n");
    g_debugUart->sendString("USART Refactored Driver Example\r\n");
    g_debugUart->sendString("STM32L433 LPUART1 @ 115200 baud\r\n");
    g_debugUart->sendString("========================================\r\n\r\n");
}

/**
 * @brief Main application event loop
 * 
 * Runs continuously after initialization.
 * Demonstrates various USART transmission methods.
 */
void App_Run(void)
{
    if (g_debugUart == nullptr) {
        return;  // Not initialized
    }
    
    // Counter for demo messages
    static uint32_t messageCount = 0;
    
    while (1) {
        // Example 1: Send simple text
        g_debugUart->sendString("--- Message ");
        g_debugUart->sendFormatted("%lu ---\r\n", messageCount);
        
        // Example 2: Send formatted strings
        g_debugUart->sendFormatted("Counter value: %lu\r\n", messageCount);
        g_debugUart->sendFormatted("Remaining buffer space: %u bytes\r\n", 
                                   g_debugUart->getAvailableSpace());
        
        // Example 3: Send hex representation
        uint8_t testData[] = {0xDE, 0xAD, 0xBE, 0xEF};
        g_debugUart->sendString("Hex representation: ");
        g_debugUart->sendHex(testData, sizeof(testData), true);
        g_debugUart->sendString("\r\n");
        
        // Example 4: Send binary representation (for first byte only to save buffer space)
        g_debugUart->sendString("Binary (first byte): ");
        g_debugUart->sendBinary(&testData[0], 1);
        g_debugUart->sendString("\r\n");
        
        // Example 5: Check transmission status
        if (g_debugUart->isTransmissionActive()) {
            g_debugUart->sendString("  [Transmission in progress...]\r\n");
        } else {
            g_debugUart->sendString("  [Transmission complete]\r\n");
        }
        
        // Add blank line
        g_debugUart->sendString("\r\n");
        
        // Wait ~500ms before next message (approximately)
        // In production, use a proper timer
        for (volatile uint32_t i = 0; i < 500000; i++) {
            __NOP();
        }
        
        messageCount++;
        
        // Safety: reset counter after 100 iterations to avoid overflow in formatting
        if (messageCount > 100) {
            messageCount = 0;
            g_debugUart->sendString("--- Counter Reset ---\r\n\r\n");
        }
    }
}

/**
 * @brief Error handler (weak symbol, can be overridden)
 */
__attribute__((weak))
void Error_Handler(void)
{
    // Blink LED or infinite loop to indicate error
    while (1) {
        __NOP();
    }
}
