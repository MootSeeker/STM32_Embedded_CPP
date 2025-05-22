#ifndef MOCK_STM32L4XX_H
#define MOCK_STM32L4XX_H

#include <cstdint>
#include <cstring> // For memset

// Basic CMSIS types (simplified for host testing)
#define     __IO    volatile
#define     __IM    volatile const
#define     __OM    volatile

// Mock GPIO_TypeDef structure
typedef struct
{
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

// Mock RCC_TypeDef structure (simplified, add fields as needed by your code)
typedef struct
{
  __IO uint32_t CR;
  uint32_t RESERVED0[11]; // Placeholder for other registers up to AHB2ENR
  __IO uint32_t AHB1ENR;
  __IO uint32_t AHB2ENR;     /*!< RCC AHB2 Peripherals Clock Enable Register,     Address offset: 0x4C */
  // Add other RCC registers if your code uses them
} RCC_TypeDef;

// Define base addresses for peripheral instances (mocked)
#ifdef UNIT_TESTING
// For mocking, define base addresses as pointers
#define GPIOA_BASE            (&mock_GPIOA_Peripheral)
#define GPIOB_BASE            (&mock_GPIOB_Peripheral)
#define GPIOC_BASE            (&mock_GPIOC_Peripheral)
#define GPIOD_BASE            (&mock_GPIOD_Peripheral)
#define GPIOE_BASE            (&mock_GPIOE_Peripheral)
#define GPIOF_BASE            (&mock_GPIOF_Peripheral)
#define GPIOG_BASE            (&mock_GPIOG_Peripheral)
#define GPIOH_BASE            (&mock_GPIOH_Peripheral)
#define RCC_BASE              (&mock_RCC_Peripheral)
#else
// Real addresses for firmware
#define GPIOA_BASE            (0x48000000UL)
#define GPIOB_BASE            (0x48000400UL)
#define GPIOC_BASE            (0x48000800UL)
#define GPIOD_BASE            (0x48000C00UL)
#define GPIOE_BASE            (0x48001000UL)
#define GPIOF_BASE            (0x48001400UL)
#define GPIOG_BASE            (0x48001800UL)
#define GPIOH_BASE            (0x48001C00UL)
#define RCC_BASE              (0x40021000UL)
#endif

#ifdef UNIT_TESTING

// Declare global mock instances (defined in mock_stm32l4xx.cpp)
extern GPIO_TypeDef mock_GPIOA_Peripheral;
extern GPIO_TypeDef mock_GPIOB_Peripheral;
extern GPIO_TypeDef mock_GPIOC_Peripheral;
extern GPIO_TypeDef mock_GPIOD_Peripheral;
extern GPIO_TypeDef mock_GPIOE_Peripheral;
extern GPIO_TypeDef mock_GPIOF_Peripheral;
extern GPIO_TypeDef mock_GPIOG_Peripheral;
extern GPIO_TypeDef mock_GPIOH_Peripheral;

extern RCC_TypeDef mock_RCC_Peripheral;

// Redefine peripheral pointers to point to our mock instances for test builds
#define GPIOA               (&mock_GPIOA_Peripheral)
#define GPIOB               (&mock_GPIOB_Peripheral)
#define GPIOC               (&mock_GPIOC_Peripheral)
#define GPIOD               (&mock_GPIOD_Peripheral)
#define GPIOE               (&mock_GPIOE_Peripheral)
#define GPIOF               (&mock_GPIOF_Peripheral)
#define GPIOG               (&mock_GPIOG_Peripheral)
#define GPIOH               (&mock_GPIOH_Peripheral)

#define RCC                 (&mock_RCC_Peripheral)

// Mock necessary RCC register bits (e.g., for enabling GPIO clocks)
// These should match the bit definitions in the real stm32l4xx.h
#define RCC_AHB2ENR_GPIOAEN_Pos    (0U)
#define RCC_AHB2ENR_GPIOAEN_Msk    (0x1UL << RCC_AHB2ENR_GPIOAEN_Pos)
#define RCC_AHB2ENR_GPIOAEN        RCC_AHB2ENR_GPIOAEN_Msk

#define RCC_AHB2ENR_GPIOBEN_Pos    (1U)
#define RCC_AHB2ENR_GPIOBEN_Msk    (0x1UL << RCC_AHB2ENR_GPIOBEN_Pos)
#define RCC_AHB2ENR_GPIOBEN        RCC_AHB2ENR_GPIOBEN_Msk

#define RCC_AHB2ENR_GPIOCEN_Pos    (2U)
#define RCC_AHB2ENR_GPIOCEN_Msk    (0x1UL << RCC_AHB2ENR_GPIOCEN_Pos)
#define RCC_AHB2ENR_GPIOCEN        RCC_AHB2ENR_GPIOCEN_Msk

#define RCC_AHB2ENR_GPIODEN_Pos    (3U)
#define RCC_AHB2ENR_GPIODEN_Msk    (0x1UL << RCC_AHB2ENR_GPIODEN_Pos)
#define RCC_AHB2ENR_GPIODEN        RCC_AHB2ENR_GPIODEN_Msk

#define RCC_AHB2ENR_GPIOEEN_Pos    (4U)
#define RCC_AHB2ENR_GPIOEEN_Msk    (0x1UL << RCC_AHB2ENR_GPIOEEN_Pos)
#define RCC_AHB2ENR_GPIOEEN        RCC_AHB2ENR_GPIOEEN_Msk

#define RCC_AHB2ENR_GPIOFEN_Pos    (5U)
#define RCC_AHB2ENR_GPIOFEN_Msk    (0x1UL << RCC_AHB2ENR_GPIOFEN_Pos)
#define RCC_AHB2ENR_GPIOFEN        RCC_AHB2ENR_GPIOFEN_Msk

#define RCC_AHB2ENR_GPIOGEN_Pos    (6U)
#define RCC_AHB2ENR_GPIOGEN_Msk    (0x1UL << RCC_AHB2ENR_GPIOGEN_Pos)
#define RCC_AHB2ENR_GPIOGEN        RCC_AHB2ENR_GPIOGEN_Msk

#define RCC_AHB2ENR_GPIOHEN_Pos    (7U)
#define RCC_AHB2ENR_GPIOHEN_Msk    (0x1UL << RCC_AHB2ENR_GPIOHEN_Pos)
#define RCC_AHB2ENR_GPIOHEN        RCC_AHB2ENR_GPIOHEN_Msk

#endif // UNIT_TESTING

// Declaration for a function to reset all mock peripherals
void reset_all_mocks();

#endif // MOCK_STM32L4XX_H