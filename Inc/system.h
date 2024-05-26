/*
 * system.h
 *
 *  Created on: May 26, 2024
 *      Author: kevin
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_


#include <common.h>

#pragma once

namespace Core
{
	extern uint32_t clock;
	extern uint32_t ms_ticks;
} // namespace Core

static inline void delay_ms( uint32_t ms )
{
	uint64_t final = ms + Core::ms_ticks;
	while( Core::ms_ticks < final )
	{
		__asm("nop");
	}
}


#endif /* SYSTEM_H_ */
