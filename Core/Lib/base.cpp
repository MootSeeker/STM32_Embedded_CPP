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


unsigned bIO::read( void )
{
	return 0;
}

void bIO::write( unsigned data )
{
	__UNUSED( data );
}

void bIO::stream( volatile void* buffer, int length )
{
	__UNUSED( buffer );
	__UNUSED( length );
}




