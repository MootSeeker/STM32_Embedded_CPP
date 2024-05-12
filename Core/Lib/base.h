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
        virtual HAL_StatusTypeDef get_status( void );

    protected:
        HAL_StatusTypeDef status = HAL_ERROR; /**< Status of the peripheral. */

    private:
};

#endif /* LIB_BASE_H_ */
