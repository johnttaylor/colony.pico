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

#include "Cpl/System/GlobalLock.h"
#include "pico/sync.h"


extern critical_section_t g_cplSystemRp2040_globalCritSec_;


//////////////////////////////////////////////////////////////////////////////
void Cpl::System::GlobalLock::begin( void )
{
    critical_section_enter_blocking( &g_cplSystemRp2040_globalCritSec_ );
}

void Cpl::System::GlobalLock::end( void )
{
    critical_section_exit_blocking( &g_cplSystemRp2040_globalCritSec_ );
}



