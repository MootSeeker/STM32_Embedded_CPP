/*
 * rcc.h
 *
 *  Created on: May 21, 2024
 *      Author: kevin
 */

#ifndef RCC_H_
#define RCC_H_

#include <common.h>

class CLOCK {
public:
		CLOCK();
    void enableHSI();      // Enable High-Speed Internal clock
    void enableHSE();      // Enable High-Speed External clock
    void enablePLL();      // Enable Phase-Locked Loop
    void selectSystemClock(); // Select System Clock source
    void configurePLL();   // Configure PLL settings

private:
    void waitForClockReady(uint32_t flag); // Wait for the specified clock to be ready
};

#endif /* RCC_H_ */
