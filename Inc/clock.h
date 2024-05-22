/*
 * clock.h
 *
 *  Created on: May 22, 2024
 *      Author: kevin.perillo
 */

#ifndef CLOCK_H_
#define CLOCK_H_


#include <rcc.h>

class Clock {
public:
    Clock();
    void configureSystemClockTo64MHz();
private:
    rcc rccController;
};


#endif /* CLOCK_H_ */
