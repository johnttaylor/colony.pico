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

#include "Cpl/System/Api.h"
#include "Cpl/System/Private_.h"
#include "pico/sync.h"

/// 
using namespace Cpl::System;


///
static Mutex systemMutex_;
static Mutex tracingMutex_;
static Mutex tracingOutputMutex_;
static Mutex sysListMutex_;


////////////////////////////////////////////////////////////////////////////////
void Api::initialize( void )
{
    // Init the Colony.Core sub-systems
    StartupHook_::notifyStartupClients();
}


void Api::enableScheduling( void )
{
    // Nothing needed!
}

bool Api::isSchedulingEnabled( void )
{
    return true; // Always return true since scheduling does not has a real meaning for our Dual Thread/Core system
}

void Api::sleep( unsigned long milliseconds ) noexcept
{
    busy_wait_ms( milliseconds );
}

void Api::suspendScheduling(void)
{
    // DO NOT Nothing.  Has no meaning for a bare metal system
}

void Api::resumeScheduling(void)
{
    // DO NOT Nothing.  See above
}

////////////////////////////////////////////////////////////////////////////////
Mutex& Locks_::system( void )
{
    return systemMutex_;
}

Mutex& Locks_::sysLists( void )
{
    return sysListMutex_;
}

Mutex& Locks_::tracing( void )
{
    return tracingMutex_;
}

Mutex& Locks_::tracingOutput( void )
{
    return tracingOutputMutex_;
}

