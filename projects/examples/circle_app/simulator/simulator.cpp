/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

/*
Create TPipe
project:main supplies the socket connector
Button Frame Handler
    -->implements the HAL layer for Button driver
Create Thread
    Create scheduling

*/
#include "../app.h"
#include "colony_config.h"
#include "Driver/TPipe/Maker.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Dm/PeriodicScheduler.h"
#include "Driver/Button/TPipe/Hal.h"

#ifndef OPTION_MY_APP_SIM_RX_FRAME_HANDLER_SIZE
#define OPTION_MY_APP_SIM_RX_FRAME_HANDLER_SIZE     256
#endif

static Cpl::Container::Map<Driver::TPipe::RxFrameHandlerApi> frameHandlers_( "ignoreThisParameter_usedToSelecStaticContructor" );
static Driver::TPipe::Maker                                  tpipe_( frameHandlers_, OPTION_MY_APP_SIM_RX_FRAME_HANDLER_SIZE );

static Cpl::Io::Input*  tpipeInfd_;
static Cpl::Io::Output* tpipeOutfd_;

// Create the Button drivers
Driver::Button::PolledDebounced g_buttonA( "A" );
Driver::Button::PolledDebounced g_buttonB( "B" );
Driver::Button::PolledDebounced g_buttonX( "X" );
Driver::Button::PolledDebounced g_buttonY( "Y" );


/*-----------------------------------------------------------*/
//
// Set up Periodic scheduling for the TPipe thread
//

static Cpl::System::PeriodicScheduler::Interval_T tpipeIntervals_[] =
{
    /* No intervals because we are not (currently) sharing the thread. 
       The TPipe only needs/uses the 'Idle function' 
    */
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS
};

// In thread initialization for code the executes in the TPipe thread
void tpipeStart( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    tpipe_.getPipeProcessor().start( *tpipeInfd_, *tpipeOutfd_ );
}

// Idle function for the TPipe thread
void tpipeIdle( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    tpipe_.getPipeProcessor().poll();
}

// Runnable object for the TPipe thread 
Cpl::Dm::PeriodicScheduler tpipeMbox_( tpipeIntervals_, 
                                       tpipeStart, 
                                       nullptr, 
                                       nullptr, 
                                       Cpl::System::ElapsedTime::precision, 
                                       tpipeIdle );


/*-----------------------------------------------------------*/
void platform_init( Cpl::Io::Input& tpipeInStream, Cpl::Io::Output& tpipeOutStream )
{
    // Initialize the HAL for the button driver
    driverButtonHalTPipe_initialize( frameHandlers_ );

    // Cache the TPipe Streams 
    tpipeInfd_  = &tpipeInStream;
    tpipeOutfd_ = &tpipeOutStream;

    // Create thread for the TPipe
    Cpl::System::Thread::create( tpipeMbox_, "TPIPE" );
}

