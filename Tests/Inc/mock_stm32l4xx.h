#ifndef MOCK_STM32L4XX_H
#define MOCK_STM32L4XX_H

#include <cstdint>
#include <cstring> // For memset

// Basic CMSIS types (simplified for host testing)
#define     __IO    volatile
#define     __IM    volatile const
#define     __OM    volatile

// GPIO Enable bits for RCC AHB2ENR
#define RCC_AHB2ENR_GPIOAEN     (1UL << 0)
#define RCC_AHB2ENR_GPIOBEN     (1UL << 1)
#define RCC_AHB2ENR_GPIOCEN     (1UL << 2)
#define RCC_AHB2ENR_GPIODEN     (1UL << 3)
#define RCC_AHB2ENR_GPIOEEN     (1UL << 4)
#define RCC_AHB2ENR_GPIOFEN     (1UL << 5)
#define RCC_AHB2ENR_GPIOGEN     (1UL << 6)
#define RCC_AHB2ENR_GPIOHEN     (1UL << 7)

// Mock GPIO_TypeDef structure
typedef struct {
    __IO uint32_t MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
    __IO uint32_t OTYPER;   /*!< GPIO port output type register,        Address offset: 0x04      */
    __IO uint32_t OSPEEDR;  /*!< GPIO port output speed register,       Address offset: 0x08      */
    __IO uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
    __IO uint32_t IDR;      /*!< GPIO port input data register,         Address offset: 0x10      */
    __IO uint32_t ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
    __IO uint32_t BSRR;     /*!< GPIO port bit set/reset register,      Address offset: 0x18      */
    __IO uint32_t LCKR;     /*!< GPIO port configuration lock register, Address offset: 0x1C      */
    __IO uint32_t AFR[2];   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
    __IO uint32_t BRR;      /*!< GPIO port bit reset register,          Address offset: 0x28      */
} GPIO_TypeDef;

// Mock RCC_TypeDef structure 
typedef struct {
    __IO uint32_t CR;          /*!< RCC clock control register,                 Address offset: 0x00 */
    uint32_t RESERVED0[11];    /*!< Reserved space to match real hardware offset */
    __IO uint32_t AHB1ENR;     /*!< RCC AHB1 peripheral clock enable register, Address offset: 0x30 */
    __IO uint32_t AHB2ENR;     /*!< RCC AHB2 peripheral clock enable register, Address offset: 0x34 */
} RCC_TypeDef;

// External declarations for mock peripherals
extern GPIO_TypeDef mock_GPIOA_Peripheral;
extern GPIO_TypeDef mock_GPIOB_Peripheral;
extern GPIO_TypeDef mock_GPIOC_Peripheral;
extern GPIO_TypeDef mock_GPIOD_Peripheral;
extern GPIO_TypeDef mock_GPIOE_Peripheral;
extern GPIO_TypeDef mock_GPIOF_Peripheral;
extern GPIO_TypeDef mock_GPIOG_Peripheral;
extern GPIO_TypeDef mock_GPIOH_Peripheral;
extern RCC_TypeDef mock_RCC_Peripheral;

// Define peripheral pointers for test environment
#define GPIOA           ((GPIO_TypeDef*)&mock_GPIOA_Peripheral)
#define GPIOB           ((GPIO_TypeDef*)&mock_GPIOB_Peripheral)
#define GPIOC           ((GPIO_TypeDef*)&mock_GPIOC_Peripheral)
#define GPIOD           ((GPIO_TypeDef*)&mock_GPIOD_Peripheral)
#define GPIOE           ((GPIO_TypeDef*)&mock_GPIOE_Peripheral)
#define GPIOF           ((GPIO_TypeDef*)&mock_GPIOF_Peripheral)
#define GPIOG           ((GPIO_TypeDef*)&mock_GPIOG_Peripheral)
#define GPIOH           ((GPIO_TypeDef*)&mock_GPIOH_Peripheral)
#define RCC             ((RCC_TypeDef*)&mock_RCC_Peripheral)

// Base addresses (used for firmware but defined here for compatibility)
#define GPIOA_BASE            ((uint32_t)GPIOA)
#define GPIOB_BASE            ((uint32_t)GPIOB)
#define GPIOC_BASE            ((uint32_t)GPIOC)
#define GPIOD_BASE            ((uint32_t)GPIOD)
#define GPIOE_BASE            ((uint32_t)GPIOE)
#define GPIOF_BASE            ((uint32_t)GPIOF)
#define GPIOG_BASE            ((uint32_t)GPIOG)
#define GPIOH_BASE            ((uint32_t)GPIOH)
#define RCC_BASE              ((uint32_t)RCC)

// Function to reset all mocks to their initial state
void reset_all_mocks(void);

#endif // MOCK_STM32L4XX_H