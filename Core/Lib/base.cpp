/*
 * base.cpp
 *
 *  Created on: May 12, 2024
 *      Author: kevin
 */

#include "base.h"

// Constructor
bIO::bIO() {}

// Destructor
bIO::~bIO() {}

/**
 * @brief General read function.
 * @return The read value.
 */
unsigned bIO::read( void )
{
    return 0; // Placeholder return value
}

/**
 * @brief General write function.
 * @param data The data to be written.
 */
void bIO::write( unsigned data )
{
    __UNUSED( data ); // Unused parameter
}

/**
 * @brief Stream data.
 * @param buffer Pointer to the data buffer.
 * @param length Length of the buffer.
 */
void bIO::stream( volatile void* buffer, int length )
{
    __UNUSED( buffer ); // Unused parameter
    __UNUSED( length ); // Unused parameter
}

/**
 * @brief Initialize the peripheral.
 * This function will be overwritten by subclass.
 */
void bIO::init( void )
{
    // Do nothing
}

/**
 * @brief Reset the peripheral.
 * This function will be overwritten by subclass.
 */
void bIO_reset( void )
{
    // Do nothing
}

/**
 * @brief Disable the peripheral.
 * This function will be overwritten by subclass.
 */
void bIO::disable( void )
{
    // Do nothing
}

/**
 * @brief Get the status of the peripheral.
 * @return The status of the peripheral.
 */
HAL_StatusTypeDef bIO::get_status( void )
{
    return this->status; // Return the status
}
