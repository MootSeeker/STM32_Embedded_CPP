/*
 * common.h
 *
 *  Created on: May 21, 2024
 *      Author: kevin
 */

#ifndef COMMON_H_
#define COMMON_H_


#ifdef STM32L432KC
#include <stm32l432xx.h>
#include <stm32l4xx.h>
#include <system_stm32l4xx.h>
#endif


#ifdef __stm32f0
#include "stm32f0/stm32f0xx.h"
#elif defined __stm32f1
#include "stm32f1/stm32f1xx.h"
#endif

#ifdef __stm32f3
#include "stm32f3/stm32f3xx.h"
#endif

#ifdef __stm32f4
#include "stm32f4/stm32f4xx.h"
#endif

#ifdef __stm32f7
#include "stm32f7/stm32f7xx.h"
#endif


#ifdef __cplusplus
#include <cstdint>
#include <cstdlib>
#else
#include <stdint.h>
#include <stdlib.h>
#endif


#endif /* COMMON_H_ */
