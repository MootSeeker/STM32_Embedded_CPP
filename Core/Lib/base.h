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




// Class declaration for basic input / output structures
// Usable for GPIO, USART, SPI, I2C, etc.
class bIO
{
	public:
		bIO( );
		~bIO( );

		// General read and write functions
		virtual unsigned read( void );
		virtual void write( unsigned dat );
		virtual void stream( volatile void *buf, int len );

		// Common peripheral control methods.
		virtual void clock_en( void );
		virtual void reset( void );
		virtual void disable( void );
		virtual int get_status( void );
	protected:

		HAL_StatusTypeDef status = HAL_ERROR;

		// Enable/disable/reset register definitions.
		__IO uint32_t *enable_reg = 0;
		__IO uint32_t *reset_reg = 0;
		uint32_t enable_bit = 0;
		uint32_t reset_bit = 0;

	private:

};


#endif /* LIB_BASE_H_ */
