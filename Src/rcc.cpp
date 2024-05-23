#include <rcc.h>

CLOCK::CLOCK( )
{
	// Constructor implementation (if needed)
}

void CLOCK::enableHSI( )
{
	RCC->CR |= RCC_CR_HSION; // Enable HSI
	waitForClockReady( RCC_CR_HSIRDY ); // Wait for HSI to be ready
}

void CLOCK::enableHSE( )
{
	RCC->CR |= RCC_CR_HSEON; // Enable HSE
	waitForClockReady( RCC_CR_HSERDY ); // Wait for HSE to be ready
}

void CLOCK::enablePLL( )
{
	RCC->CR |= RCC_CR_PLLON; // Enable PLL
	waitForClockReady( RCC_CR_PLLRDY ); // Wait for PLL to be ready
}

void CLOCK::selectSystemClock( )
{
	// Example: Select PLL as system clock source
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	// Wait for PLL to be used as system clock source
	while( ( RCC->CFGR & RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL );
}

void CLOCK::configurePLL( )
{
	// Example PLL configuration (adjust as needed)
	RCC->PLLCFGR = ( 1 << RCC_PLLCFGR_PLLSRC_Pos ) |  // PLL source HSI
			( 4 << RCC_PLLCFGR_PLLM_Pos ) |    // PLLM division factor 4
			( 40 << RCC_PLLCFGR_PLLN_Pos ) |   // PLLN multiplication factor 40
			( 0 << RCC_PLLCFGR_PLLP_Pos ) |    // PLLP division factor 7 (default)
			( 2 << RCC_PLLCFGR_PLLR_Pos );     // PLLR division factor 2

	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;       // Enable PLLR output
}

void CLOCK::waitForClockReady( uint32_t flag )
{
	while( !( RCC->CR & flag ) )
	{
		// Wait until the specified clock is ready
	}
}
