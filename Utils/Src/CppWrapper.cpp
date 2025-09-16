/**
 * Filename: CppWrapper.cpp
 *
 */

#include "CppWrapper.h"
#include "main.h"


#include "App.h"

#include <new>
#include <typeindex>
#include <typeinfo>
#include <functional>
#include <map>
#include <cstdlib>
#include <cstdio>

// C++ operator new/delete overrides for ST Newlib integration
// These use standard malloc/free which are thread-safe via heap_useNewlib_ST.c

void* operator new(size_t size)
{
    void* ptr = malloc(size);
    if (!ptr)
    {
        // For embedded systems: Go into error handler to stop system
    	Error_Handler( );
    }
    return ptr;
}

void* operator new[](size_t size)
{
    void* ptr = malloc(size);
    if (!ptr)
    {
        // For embedded systems: Go into error handler to stop system
    	Error_Handler( );
    }
    return ptr;
}

void operator delete(void* ptr) noexcept
{
    if (ptr) {
        free(ptr);
    }
}

void operator delete[](void* ptr) noexcept
{
    if (ptr) {
        free(ptr);
    }
}

// C++14 sized delete operators
void operator delete(void* ptr, size_t size) noexcept
{
    (void)size; // Suppress unused parameter warning
    if (ptr) {
        free(ptr);
    }
}

void operator delete[](void* ptr, size_t size) noexcept
{
    (void)size; // Suppress unused parameter warning
    if (ptr) {
        free(ptr);
    }
}


void EventLoopC( void )
{
	App_Init( );

	App_Run( );

	return;
}

