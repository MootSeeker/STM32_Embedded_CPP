/*
 * rcc.h
 *
 *  Created on: May 21, 2024
 *      Author: kevin
 */

#ifndef RCC_H_
#define RCC_H_

#include <common.h>


class CLOCK
{
	public:
		CLOCK( );
		void enableHSI( );      // Enable High-Speed Internal clock
		void enableHSE( );      // Enable High-Speed External clock
		void enablePLL( );      // Enable Phase-Locked Loop
		void selectSystemClock( ); // Select System Clock source
		void configurePLL( );   // Configure PLL settings
		void setSystemClockTo64MHz( ); // Set system clock to 64 MHz

	private:
		void waitForClockReady( uint32_t flag ); // Wait for the specified clock to be ready
		void setFlashLatency( uint32_t latency ); // Set Flash latency
		void setVoltageScaling( uint32_t scale ); // Set Voltage scaling
};

#endif /* RCC_H_ */
