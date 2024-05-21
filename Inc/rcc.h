/*
 * rcc.h
 *
 *  Created on: May 21, 2024
 *      Author: kevin
 */

#ifndef RCC_H_
#define RCC_H_


#include <common.h>

class rcc {
	enum class GPIO_Port
	{
		GPIO_PORTA,
		GPIO_PORTB,
		GPIO_PORTC,
		GPIO_PORTD,
		GPIO_PORTE,
		GPIO_PORTF,
		GPIO_PORTG,
		GPIO_PORTH
	};

	enum class USART_Port
	{
		USART_PORT1,
		USART_PORT2,
		USART_PORT3
	};

	enum class I2C_Port
	{
		I2C_PORT1,
		I2C_PORT2,
		I2C_PORT3
	};

	enum class SPI_Port
	{
		SPI_PORT1,
		SPI_PORT2,
		SPI_PORT3
	};

	enum class ADC_Port
	{
		ADC_PORT1
	};

	enum class TIM_Port
	{
		TIM_PORT1,
		TIM_PORT2,
		TIM_PORT3,
		TIM_PORT4,
		TIM_PORT5,
		TIM_PORT6,
		TIM_PORT7,
		TIM_PORT8,
		TIM_PORT15,
		TIM_PORT16,
		TIM_PORT17
	};

	enum class DMA_Port
	{
		DMA_PORT1,
		DMA_PORT2
	};

public:
	rcc();
	void enableHSI();
	void enableHSE();
	void enableLSI();
	void enableLSE();
	void enablePLL();
	void configureSystemClock();

	void enableGPIO(GPIO_Port port);
	void enableUSART(USART_Port port);
	void enableI2C(I2C_Port port);
	void enableSPI(SPI_Port port);
	void enableADC(ADC_Port port);
	void enableTIM(TIM_Port port);
	void enableDAC();
	void enableDMA(DMA_Port port);

	void enableRTC();
	void enableIWDG();
	void enableWWDG();

private:
	void setFlashLatency();
	void configurePLL();
	void configureVoltageScaling();
	void configureOscillators();
	void configureClocks();

};

#endif /* RCC_H_ */
