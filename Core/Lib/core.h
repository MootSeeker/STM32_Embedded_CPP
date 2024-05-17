/*
 * base.h
 *
 *  Created on: May 12, 2024
 *      Author: kevin
 */

#ifndef LIB_CORE_H_
#define LIB_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"

#ifdef __cplusplus
}
#endif


class CORE_IO {
public:
    CORE_IO() {}
    virtual ~CORE_IO() {}

protected:
    virtual void write(uint32_t data) = 0;
    virtual uint32_t read() const = 0;
    virtual void stream(uint32_t* buffer) = 0;
};



#endif /* LIB_CORE_H_ */
