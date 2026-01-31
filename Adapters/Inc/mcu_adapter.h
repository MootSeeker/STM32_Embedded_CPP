#ifndef MCU_ADAPTER_H_
#define MCU_ADAPTER_H_

/*
 * This file abstracts the specific STM32 family headers.
 * It selects the correct LL/HAL headers based on the defined preprocessor symbol.
 */

#if defined(STM32L433xx)
    /* STM32L433 Specific Includes */
    #include "stm32l4xx_ll_bus.h"
    #include "stm32l4xx_ll_cortex.h"
    #include "stm32l4xx_ll_crs.h"
    #include "stm32l4xx_ll_dma.h"
    #include "stm32l4xx_ll_exti.h"
    #include "stm32l4xx_ll_gpio.h"
    #include "stm32l4xx_ll_lpuart.h"
    #include "stm32l4xx_ll_pwr.h"
    #include "stm32l4xx_ll_rcc.h"
    #include "stm32l4xx_ll_system.h"
    #include "stm32l4xx_ll_tim.h"
    #include "stm32l4xx_ll_utils.h"
    
#elif defined(STM32F446xx)
    /* Example for future F4 support (Nucleo-F446RE) */
    /* #include "stm32f4xx_ll_gpio.h" */
    #error "STM32F446xx not yet fully implemented in mcu_adapter.h"

#else
    /* 
     * If you see this error, you need to define the target MCU in your build configuration
     * (e.g., -DSTM32L433xx)
     */
    #error "Target MCU not defined or unsupported in mcu_adapter.h"
#endif

#endif /* MCU_ADAPTER_H_ */
