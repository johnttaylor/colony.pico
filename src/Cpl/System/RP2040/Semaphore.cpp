/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Cpl/System/Semaphore.h"
#include "Cpl/System/GlobalLock.h"
#include "Cpl/System/ElapsedTime.h"

///
using namespace Cpl::System;



//////////////////////////////////////////////////
Semaphore::Semaphore( unsigned initialCount )
{
    sem_init( &m_sema, initialCount, 0x7FFF );
}

Semaphore::~Semaphore()
{
    // Nothing needed
}

int Semaphore::signal( void ) noexcept
{
    return sem_release( &m_sema )? 0: 1;    // Return zero on success
}

int Semaphore::su_signal( void ) noexcept
{
    return sem_release( &m_sema ) ? 0 : 1;  // Return zero on success
}

bool Semaphore::tryWait( void ) noexcept
{
    return sem_try_acquire( &m_sema );
}

void Semaphore::waitInRealTime( void ) noexcept
{
    sem_acquire_blocking( &m_sema );
}

bool Semaphore::timedWaitInRealTime( unsigned long timeout ) noexcept
{
    return sem_acquire_timeout_ms( &m_sema, timeout );
}

//////////////////////////////////////////////////
// Simulated time NOT supported
bool Semaphore::timedWait( unsigned long timeout ) noexcept
{
    return sem_acquire_timeout_ms( &m_sema, timeout );
}

void Semaphore::wait( void ) noexcept
{
    sem_acquire_blocking( &m_sema );
}
