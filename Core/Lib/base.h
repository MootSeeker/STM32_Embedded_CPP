/*
 * base.h
 *
 *  Created on: May 12, 2024
 *      Author: kevin
 */

#ifndef LIB_BASE_H_
#define LIB_BASE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32l4xx_hal.h"

#ifdef __cplusplus
}
#endif

/*
 * These addresses are provided by ST's device headers already,
 * but their 'typedef' syntax can be tough to use with class vars.
 * So while these macros are a bit ungainly, we can hide them
 * away in a header file and pretend they don't exist as usual.
 *
 * This is very incomplete; I only add values as I use them.
 */
#define __REG(x) (__IO uint32_t*)((x))

// Global macro definitions.
#define pSTATUS_ERR (0)
#define pSTATUS_SET (1)
#define pSTATUS_ON  (2)
#define pSTATUS_RUN (3)


/**
 * @brief Class declaration for basic input/output structures.
 *
 * This class can be used for GPIO, USART, SPI, I2C, etc.
 */
class bIO
{
    public:
        /**
         * @brief Constructor.
         */
        bIO( );

        /**
         * @brief Destructor.
         */
        ~bIO( );

        /**
         * @brief General read function.
         * @return The read value.
         */
        virtual unsigned read( void );

        /**
         * @brief General write function.
         * @param data The data to be written.
         */
        virtual void write( unsigned data );

        /**
         * @brief Stream data.
         * @param buffer Pointer to the data buffer.
         * @param length Length of the buffer.
         */
        virtual void stream( volatile void *buffer, int length );

        /**
         * @brief Initialize the peripheral.
         */
        virtual void init( void );

        /**
         * @brief Reset the peripheral.
         */
        virtual void reset( void );

        /**
         * @brief Disable the peripheral.
         */
        virtual void disable( void );

        /**
         * @brief Get the status of the peripheral.
         * @return The status of the peripheral.
         */
        virtual int get_status( void );

    protected:
        int status = pSTATUS_ERR;

        // Enable/disable/reset register definitions.
        __IO uint32_t *enable_reg = 0;
        __IO uint32_t *reset_reg  = 0;
        uint32_t       enable_bit = 0;
        uint32_t       reset_bit  = 0;

    private:
};

#endif /* LIB_BASE_H_ */
