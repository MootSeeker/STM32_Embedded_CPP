/*
 * system.cpp
 *
 *  Created on: May 26, 2024
 *      Author: kevin
 */



#include <system.h>


uint32_t Core::clock = SystemCoreClock;
 uint32_t Core::ms_ticks = 0;

extern "C" void SysTick_Handler(void)
{
    Core::ms_ticks += 1;
}
