/*
 * common.h
 *
 *  Created on: May 21, 2024
 *      Author: kevin
 */

#ifndef COMMON_H_
#define COMMON_H_

// Include standard libraries
#ifdef __cplusplus
#include <cstdint>
#include <cstdlib>
#else
#include <stdint.h>
#include <stdlib.h>
#endif

// Include STM32 specific headers
#ifdef STM32L432KC
#define STM32L432xx
#include <stm32l432xx.h>
#include <stm32l4xx.h>
#include <system_stm32l4xx.h>
#endif





#endif /* COMMON_H_ */
