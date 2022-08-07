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

#include "colony_config.h"
#include "Cpl/TShell/PolledMaker.h"
#include "Cpl/System/Thread.h"
#include "statics.h"



/// 
extern void shell_test4( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

Cpl::Container::Map<Cpl::TShell::Command>           g_cmdlist( "ignore_this_parameter-used to invoke the static constructor" );
static Cpl::TShell::PolledMaker                     polledCmdProcessor_( g_cmdlist );


////////////////////////////////////////////////////////////////////////////////



void shell_test4( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
    // Create mock application thread
    Cpl::System::Thread::create( mockApp, "APP-BOB" );

    // 
    static bool testsHaveBeenStarted = false;
    if ( !testsHaveBeenStarted )
    {
        testsHaveBeenStarted = true;

        // Start the shell
        shell_.launch( infd, outfd );

        // Create thread for my mock-application to run in
        Cpl::System::Thread::create( mockApp, "APP-BOB", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );
    }
}


