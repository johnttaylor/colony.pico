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
#include "Cpl/Itc/PeriodicScheduler.h"
#include "statics.h"



/// 
extern void shell_test4( Cpl::Io::Input& infd, Cpl::Io::Output& outfd );

static Cpl::TShell::PolledMaker                     polledCmdProcessor_( cmdlist );

//////////////////////////////////////////////
static void tshellScan_( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    polledCmdProcessor_.getCommandProcessor().poll();
}

static Cpl::System::PeriodicScheduler::Interval_T intervals_[] =
{
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS // Create an empty set of intervals - actual interval not needed for the test
};

static Cpl::Itc::PeriodicScheduler scheduler_( intervals_, nullptr, Cpl::System::ElapsedTime::precision, tshellScan_ );

////////////////////////////////////////////////////////////////////////////////



void shell_test4( Cpl::Io::Input& infd, Cpl::Io::Output& outfd )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Enter: shell_test4" ) );

    // Create mock application thread 
    Cpl::System::Thread::create( mockApp, "APP-BOB" );

    // Create a 'Scheduler' thread for the TShell to run in
    Cpl::System::Thread::create( scheduler_, "TSHELL" );

    // Start the shell
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Starting TShell...") );

    polledCmdProcessor_.getCommandProcessor().start( infd, outfd ); // Note: I don't need to set the 'blocking flag' because the processor knows it is non-blocking processor

    // Start scheduling
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Enabling scheduling") );
    Cpl::System::Api::enableScheduling();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Scheduling Enabled!") );

    // The test assumes that enableScheduling() never returns -->so make sure we don't return
    for ( ;;)
    {
        Cpl::System::Api::sleep( 1000 );
    }
}


