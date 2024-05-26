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
	// Select PLL as system clock source
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	// Wait for PLL to be used as system clock source
	while( ( RCC->CFGR & RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL );
}

void CLOCK::configurePLL( )
{
	// Example PLL configuration for 64 MHz (assuming HSI as source, 16 MHz)
	RCC->PLLCFGR = ( RCC_PLLCFGR_PLLSRC_HSI ) |  // PLL source HSI
			( 1 << RCC_PLLCFGR_PLLM_Pos ) |    // PLLM division factor 1
			( 8 << RCC_PLLCFGR_PLLN_Pos ) |   // PLLN multiplication factor 8
			( 0 << RCC_PLLCFGR_PLLP_Pos ) | // PLLP division factor 7 (default)
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

void CLOCK::setFlashLatency( uint32_t latency )
{
	FLASH->ACR = ( FLASH->ACR & ~FLASH_ACR_LATENCY ) | latency;
	while( ( FLASH->ACR & FLASH_ACR_LATENCY ) != latency );
}

void CLOCK::setVoltageScaling( uint32_t scale )
{
	PWR->CR1 = ( PWR->CR1 & ~PWR_CR1_VOS ) | scale;
	while( PWR->SR2 & PWR_SR2_VOSF );
}

void CLOCK::setSystemClockTo64MHz( )
{
	// Set flash latency
	setFlashLatency( 3 ); // Set Flash latency to 3 wait states

	// Set voltage scaling
	setVoltageScaling( PWR_CR1_VOS_0 ); // Voltage scaling range 1

	// Enable HSI
	enableHSI( );

	// Configure PLL
	configurePLL( );

	// Enable PLL
	enablePLL( );

	// Select PLL as system clock source
	selectSystemClock( );

	// Set AHB, APB1, and APB2 prescalers to 1
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

	// Configure SysTick to 1ms
	SysTick_Config( 64000000 / 1000 );

	// Set SystemCoreClock to 64 MHz
	SystemCoreClock = 64000000;
}
