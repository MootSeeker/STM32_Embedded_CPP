#include "mock_stm32l4xx.h" // For the declarations and types

#ifdef UNIT_TESTING // Ensure these are only defined for test builds

// Define the actual mock peripheral instances
GPIO_TypeDef mock_GPIOA_Peripheral;
GPIO_TypeDef mock_GPIOB_Peripheral;
GPIO_TypeDef mock_GPIOC_Peripheral;
GPIO_TypeDef mock_GPIOD_Peripheral;
GPIO_TypeDef mock_GPIOE_Peripheral;
GPIO_TypeDef mock_GPIOF_Peripheral;
GPIO_TypeDef mock_GPIOG_Peripheral;
GPIO_TypeDef mock_GPIOH_Peripheral;

RCC_TypeDef mock_RCC_Peripheral;

void reset_all_mocks() {
    // Reset all GPIO ports
    memset(&mock_GPIOA_Peripheral, 0, sizeof(GPIO_TypeDef));
    memset(&mock_GPIOB_Peripheral, 0, sizeof(GPIO_TypeDef));
    memset(&mock_GPIOC_Peripheral, 0, sizeof(GPIO_TypeDef));
    memset(&mock_GPIOD_Peripheral, 0, sizeof(GPIO_TypeDef));
    memset(&mock_GPIOE_Peripheral, 0, sizeof(GPIO_TypeDef));
    memset(&mock_GPIOF_Peripheral, 0, sizeof(GPIO_TypeDef));
    memset(&mock_GPIOG_Peripheral, 0, sizeof(GPIO_TypeDef));
    memset(&mock_GPIOH_Peripheral, 0, sizeof(GPIO_TypeDef));

    // Reset RCC
    memset(&mock_RCC_Peripheral, 0, sizeof(RCC_TypeDef));
    
    // Set default reset values if necessary
    // Example: mock_RCC_Peripheral.CR = 0x00000063; // Example reset value
}

#else
// Provide a dummy implementation for reset_all_mocks if this file were somehow
// compiled in a non-UNIT_TESTING context, to avoid linker errors.
// However, this file should typically only be part of the test build.
void reset_all_mocks() {
    // This function should not be called in firmware.
}
#endif // UNIT_TESTING