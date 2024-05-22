#include <rcc.h>

rcc::rcc( )
{
	configureVoltageScaling( );
	configureOscillators( );
	setFlashLatency( );
	configurePLL( );
	configureClocks( );
}

void rcc::enableHSI( )
{
	RCC->CR |= RCC_CR_HSION; // Enable HSI
	while( !( RCC->CR & RCC_CR_HSIRDY ) ); // Wait for HSI to be ready
}

void rcc::enableHSE( )
{
	RCC->CR |= RCC_CR_HSEON; // Enable HSE
	while( !( RCC->CR & RCC_CR_HSERDY ) ); // Wait for HSE to be ready
}

void rcc::enableLSE( )
{
	RCC->BDCR |= RCC_BDCR_LSEON; // Enable LSE
	while( !( RCC->BDCR & RCC_BDCR_LSERDY ) ); // Wait for LSE to be ready
}

void rcc::enableLSI( )
{
	RCC->CSR |= RCC_CSR_LSION; // Enable LSI
	while( !( RCC->CSR & RCC_CSR_LSIRDY ) ); // Wait for LSI to be ready
}

void rcc::setFlashLatency( )
{
	FLASH->ACR |= FLASH_ACR_LATENCY_4WS; // Set Flash latency to 4 wait states
}

void rcc::configurePLL( )
{
	// Ensure that HSI is selected as the PLL source
	RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSI;

	// Set the PLLM division factor to 1 (PLLM = 1)
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_0;

	// Set the PLLN multiplication factor to 8 (PLLN = 8)
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
	RCC->PLLCFGR |= ( 8 << RCC_PLLCFGR_PLLN_Pos );

	// Set the PLLR division factor to 2 (PLLR = 2)
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLR_0;

	// Set the PLLP division factor to 7 (PLLP = 7)
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;
	RCC->PLLCFGR |= ( 7 << RCC_PLLCFGR_PLLP_Pos );

	// Set the PLLQ division factor to 2 (PLLQ = 2)
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLQ_0;

	// Enable the PLL
	RCC->CR |= RCC_CR_PLLON;

	// Wait for the PLL to be ready
	while( !( RCC->CR & RCC_CR_PLLRDY ) );
}

void rcc::configureVoltageScaling( )
{
	// Configure the main internal regulator output voltage
	PWR->CR1 |= PWR_CR1_VOS_0; // Set voltage scaling to Scale 1
	while( !( PWR->SR2 & PWR_SR2_VOSF ) ); // Wait for voltage scaling to be ready
}

void rcc::configureOscillators( )
{
	// Enable HSI, MSI, and LSI
	RCC->CR |= RCC_CR_HSION | RCC_CR_MSION;
	RCC->CSR |= RCC_CSR_LSION; // LSI is controlled by CSR register

	// Wait for HSI, MSI, and LSI to be ready
	while( !( RCC->CR & RCC_CR_HSIRDY ) ); // Wait for HSI to be ready
	while( !( RCC->CR & RCC_CR_MSIRDY ) ); // Wait for MSI to be ready
	while( !( RCC->CSR & RCC_CSR_LSIRDY ) ); // Wait for LSI to be ready

	// Configure MSI range and calibration
	RCC->CR = ( RCC->CR & ~RCC_CR_MSIRANGE ) | RCC_CR_MSIRANGE_11; // Set MSI range to 48 MHz

	// Configure HSI Calibration
	RCC->ICSCR = ( RCC->ICSCR & ~RCC_ICSCR_HSITRIM ) | ( 16 << RCC_ICSCR_HSITRIM_Pos ); // Set HSI calibration default
}

void rcc::configureClocks( )
{
	// Initializes the CPU, AHB, and APB buses clocks
	RCC->CFGR = RCC_CFGR_SW_PLL | RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV1 | RCC_CFGR_PPRE2_DIV1; // Configure system clocks
	while( ( RCC->CFGR & RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL ); // Wait for PLL to be used as system clock source
}

void rcc::enableGPIO( GPIO_Port port )
{
	switch ( port )
	{
		case GPIO_Port::GPIO_PORTA:
			RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
		break;
		case GPIO_Port::GPIO_PORTB:
			RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
		break;
		case GPIO_Port::GPIO_PORTC:
			RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
		break;
		case GPIO_Port::GPIO_PORTD:
#ifdef RCC_AHB2ENR_GPIODEN
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
#endif
		break;
		case GPIO_Port::GPIO_PORTE:
#ifdef RCC_AHB2ENR_GPIOEEN
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
#endif
		break;
		case GPIO_Port::GPIO_PORTF:
#ifdef RCC_AHB2ENR_GPIOFEN
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOFEN;
#endif
		break;
		case GPIO_Port::GPIO_PORTG:
#ifdef RCC_AHB2ENR_GPIOGEN
            RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;
#endif
		break;
		case GPIO_Port::GPIO_PORTH:
			RCC->AHB2ENR |= RCC_AHB2ENR_GPIOHEN;
		break;
	}
}

void rcc::enableUSART( USART_Port port )
{
	switch ( port )
	{
		case USART_Port::USART_PORT1:
			RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		break;
		case USART_Port::USART_PORT2:
			RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
		break;
		case USART_Port::USART_PORT3:
#ifdef RCC_APB1ENR1_USART3EN
            RCC->APB1ENR1 |= RCC_APB1ENR1_USART3EN;
#endif
		break;
	}
}

void rcc::enableI2C( I2C_Port port )
{
	switch ( port )
	{
		case I2C_Port::I2C_PORT1:
			RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;
		break;
		case I2C_Port::I2C_PORT2:
#ifdef RCC_APB1ENR1_I2C2EN
            RCC->APB1ENR1 |= RCC_APB1ENR1_I2C2EN;
#endif
		break;
		case I2C_Port::I2C_PORT3:
			RCC->APB1ENR1 |= RCC_APB1ENR1_I2C3EN;
		break;
	}
}

void rcc::enableSPI( SPI_Port port )
{
	switch ( port )
	{
		case SPI_Port::SPI_PORT1:
			RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
		break;
		case SPI_Port::SPI_PORT2:
#ifdef RCC_APB1ENR1_SPI2EN
            RCC->APB1ENR1 |= RCC_APB1ENR1_SPI2EN;
#endif
		break;
		case SPI_Port::SPI_PORT3:
			RCC->APB1ENR1 |= RCC_APB1ENR1_SPI3EN;
		break;
	}
}

void rcc::enableADC( ADC_Port port )
{
	switch ( port )
	{
		case ADC_Port::ADC_PORT1:
			RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
		break;
	}
}

void rcc::enableTIM( TIM_Port port )
{
	switch ( port )
	{
		case TIM_Port::TIM_PORT1:
			RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
		break;
		case TIM_Port::TIM_PORT2:
			RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
		break;
		case TIM_Port::TIM_PORT3:
#ifdef RCC_APB1ENR1_TIM3EN
            RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
#endif
		break;
		case TIM_Port::TIM_PORT4:
#ifdef RCC_APB1ENR1_TIM4EN
            RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
#endif
		break;
		case TIM_Port::TIM_PORT5:
#ifdef RCC_APB1ENR1_TIM5EN
            RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
#endif
		break;
		case TIM_Port::TIM_PORT6:
			RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
		break;
		case TIM_Port::TIM_PORT7:
			RCC->APB1ENR1 |= RCC_APB1ENR1_TIM7EN;
		break;
		case TIM_Port::TIM_PORT8:
#ifdef RCC_APB2ENR_TIM8EN
            RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
#endif
		break;
		case TIM_Port::TIM_PORT15:
			RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
		break;
		case TIM_Port::TIM_PORT16:
			RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
		break;
		case TIM_Port::TIM_PORT17:
#ifdef RCC_APB2ENR_TIM17EN
            RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
#endif
		break;
	}
}

void rcc::enableDAC( )
{
	RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN; // Enable DAC clock
}

void rcc::enableDMA( DMA_Port port )
{
	switch ( port )
	{
		case DMA_Port::DMA_PORT1:
			RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
		break;
		case DMA_Port::DMA_PORT2:
			RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
		break;
	}
}

void rcc::enableRTC( )
{
	RCC->BDCR |= RCC_BDCR_RTCEN; // Enable RTC clock
}

void rcc::enableIWDG( )
{
	// IWDG is enabled by writing to its control register
	IWDG->KR = 0xCCCC; // Enable IWDG
}

void rcc::enableWWDG( )
{
	RCC->APB1ENR1 |= RCC_APB1ENR1_WWDGEN; // Enable WWDG clock
}
