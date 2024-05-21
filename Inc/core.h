/*
 * core.h
 *
 *  Created on: May 21, 2024
 *      Author: kevin.perillo
 */

#ifndef CORE_H_
#define CORE_H_


#include "app.h"







class CORE
{
public:
    CORE() = default;
    virtual ~CORE() = default;

protected:

    virtual void Init() = 0;
    virtual void Deinit() = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;

    virtual unsigned read() = 0;
    virtual void readStream(uint8_t* buffer, size_t length) = 0;
    virtual void write(uint8_t data) = 0;
    virtual void writeStream(const uint8_t* data, size_t length) = 0;

private:

};



#endif /* CORE_H_ */
