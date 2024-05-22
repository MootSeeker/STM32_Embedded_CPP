/*
 * clock.cpp
 *
 *  Created on: May 22, 2024
 *      Author: kevin.perillo
 */


#include <common.h>
#include <clock.h>

Clock::Clock( )
{
	configureSystemClockTo64MHz( );
}

void Clock::configureSystemClockTo64MHz( )
{
	// Disable all unnecessary oscillators
	    RCC->CR &= ~RCC_CR_HSEON;  // Disable HSE
	    RCC->CR &= ~RCC_CR_MSION;  // Disable MSI
	    RCC->BDCR &= ~RCC_BDCR_LSEON;  // Disable LSE
	    RCC->CSR &= ~RCC_CSR_LSION;  // Disable LSI

	    // Enable HSI
	    rccController.enableHSI();

	    // Ensure HSI is used as PLL source
	    RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLSRC) | RCC_PLLCFGR_PLLSRC_HSI;

	    // Set PLLM to 1 (PLLM = 1)
	    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	    RCC->PLLCFGR |= (1 << RCC_PLLCFGR_PLLM_Pos);

	    // Set PLLN to 16 (PLLN = 16)
	    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
	    RCC->PLLCFGR |= (16 << RCC_PLLCFGR_PLLN_Pos);

	    // Set PLLR to 2 (PLLR = 2)
	    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR;
	    RCC->PLLCFGR |= (2 << RCC_PLLCFGR_PLLR_Pos);

	    // Set PLLP to 7 (PLLP = 7)
	    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;
	    RCC->PLLCFGR |= (7 << RCC_PLLCFGR_PLLP_Pos);

	    // Set PLLQ to 2 (PLLQ = 2)
	    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;
	    RCC->PLLCFGR |= (2 << RCC_PLLCFGR_PLLQ_Pos);

	    // Enable the PLL
	    RCC->CR |= RCC_CR_PLLON;

	    // Wait for PLL to be ready
	    while (!(RCC->CR & RCC_CR_PLLRDY));

	    // Set Flash latency to 4 wait states
	    rccController.setFlashLatency();

	    // Set PLL as system clock source
	    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;

	    // Wait until PLL is used as system clock source
	    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

