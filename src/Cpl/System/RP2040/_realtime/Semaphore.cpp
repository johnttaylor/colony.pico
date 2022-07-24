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

///
using namespace Cpl::System;


//////////////////////////////////////////////////
bool Semaphore::timedWait( unsigned long timeout ) noexcept
{
    return sem_acquire_timeout_ms( &m_sema, timeout );
}

void Semaphore::wait( void ) noexcept
{
    sem_acquire_blocking( &m_sema );
}

