#include "application.h"
#include "Bob.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"

#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/Dm/PeriodicScheduler.h"
#include "Cpl/Dm/Mp/Uint32.h"
#include "Cpl/Dm/Mp/Bool.h"

#include "Cpl/TShell/PolledMaker.h"
#include "Cpl/TShell/Cmd/Help.h"
#include "Cpl/TShell/Cmd/Bye.h"
#include "Cpl/TShell/Cmd/Trace.h"
#include "Cpl/TShell/Cmd/TPrint.h"
#include "Cpl/TShell/Cmd/Threads.h"


// Allocate/create my Data Model Database and create the model points
static Cpl::Dm::ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
static Cpl::Dm::Mp::Uint32       bobDelayTimeMs_( modelDb_, "delayTime", 250 );
static Cpl::Dm::Mp::Bool         bobVerbose_( modelDb_, "verbose", false );

// Create the Command/Debug Console and populate with commands
static Cpl::Container::Map<Cpl::TShell::Command>    cmdlist_( "ignore_this_parameter-used to invoke the static constructor" );
static Cpl::TShell::PolledMaker                     cmdProcessor_( cmdlist_ );
static Cpl::TShell::Cmd::Help	                    helpCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Bye	                    byeCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Trace	                    traceCmd_( cmdlist_ );
static Cpl::TShell::Cmd::TPrint	                    tprintCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Threads                    threadsCmd_( cmdlist_ );





/*-----------------------------------------------------------*/
static Cpl::System::PeriodicScheduler::Interval_T core0Intervals_[] =
{
    { Bob::periodicInterval, { 0,1 }, nullptr  },
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS 
};

// In thread initialization for code the executes on Core 0
static void core0Start( Cpl::System::ElapsedTime::Precision_T currentTick );

// Runnable object for Core0 (aka the thread's 'main loop')
Cpl::Dm::PeriodicScheduler core0Mbox_( core0Intervals_, core0Start );
static Bob theBob_( core0Mbox_, bobDelayTimeMs_, bobVerbose_ );

void core0Start( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    core0Intervals_[0].context = &theBob_;
    theBob_.start();
}

/*-----------------------------------------------------------*/
// In thread initialization for code the executes on Core 1
static void core1Start( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    cmdProcessor_.getCommandProcessor().start( *g_consoleInputFdPtr, *g_consoleOutputFdPtr ); // Note: I don't need to set the 'blocking flag' because the processor knows it is non-blocking processor
}

static Cpl::System::PeriodicScheduler::Interval_T core1Intervals_[] =
{
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS // Create an empty set of intervals - nothing that executes in thread uses Periodic Scheduling
};

static void tshellScan_( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    cmdProcessor_.getCommandProcessor().poll();
}

// Runnable object for Core1 (aka the thread's 'main loop')
Cpl::Dm::PeriodicScheduler core1Mbox_( core1Intervals_, core1Start, nullptr, nullptr, Cpl::System::ElapsedTime::precision, tshellScan_ );


/*-----------------------------------------------------------*/
void runApplication()
{
    CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Hello.  I am the Bob example application.") );

    // Create mock application thread 
    Cpl::System::Thread::create( core0Mbox_, "APP-BOB" );

    // Create a 'Scheduler' thread for the TShell to run in
    Cpl::System::Thread::create( core1Mbox_, "TSHELL" );

    // Start scheduling
    CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Enabling scheduling") );
    Cpl::System::Api::enableScheduling();
}