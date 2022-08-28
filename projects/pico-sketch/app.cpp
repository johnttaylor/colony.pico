#include "app.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Dm/PeriodicScheduler.h"
#include "Driver/Button/PolledDebounced.h"
#include "Cpl/Dm/ModelDatabase.h"
#include "Cpl/TShell/PolledMaker.h"
#include "Cpl/TShell/Cmd/Help.h"
#include "Cpl/TShell/Cmd/Bye.h"
#include "Cpl/TShell/Cmd/Trace.h"
#include "Cpl/TShell/Cmd/TPrint.h"
#include "Cpl/TShell/Cmd/Threads.h"
#include "Cpl/Dm/TShell/Dm.h"
#include "Cpl/Dm/Mp/Uint32.h"

/*-----------------------------------------------------------*/
// Allocate/create my Data Model Database and create the model points
static Cpl::Dm::ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
static Cpl::Dm::Mp::Uint32       bobDelayTimeMs_( modelDb_, "delayTime", 250 );
//static Cpl::Dm::Mp::Bool         bobVerbose_( modelDb_, "verbose", false );

// Create the Command/Debug Console and populate with commands
static Cpl::Container::Map<Cpl::TShell::Command>    cmdlist_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
static Cpl::TShell::PolledMaker                     cmdProcessor_( cmdlist_ );
static Cpl::TShell::Cmd::Help	                    helpCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Bye	                    byeCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Trace	                    traceCmd_( cmdlist_ );
static Cpl::TShell::Cmd::TPrint	                    tprintCmd_( cmdlist_ );
static Cpl::TShell::Cmd::Threads                    threadsCmd_( cmdlist_ );
static Cpl::Dm::TShell::Dm                          dmCmd_( cmdlist_, modelDb_ );


/*-----------------------------------------------------------*/
// Graphics library: Use RGB332 mode (256 colours) on the Target to limit RAM usage
#ifndef USE_PICO_SKETCH_RGB565
pimoroni::PicoGraphics_PenRGB332 graphics_( 240, 135, nullptr );

// Graphics library: Use RGB565 mode (64K colours) on the Simulator because RAM is NOT an issue
#else
pimoroni::PicoGraphics_PenRGB565 graphics_( 240, 135, nullptr );
#endif


/*-----------------------------------------------------------*/
/// 100Hz
static void interval_10ms( Cpl::System::ElapsedTime::Precision_T currentTick,
                           Cpl::System::ElapsedTime::Precision_T currentInterval,
                           void*                                 context_notUsed )
{
    // De-bounce the button Inputs
    g_buttonA.sample();
    g_buttonB.sample();
    g_buttonX.sample();
    g_buttonY.sample();
}

/// 10Hz
static void interval_100ms( Cpl::System::ElapsedTime::Precision_T currentTick,
                            Cpl::System::ElapsedTime::Precision_T currentInterval,
                            void*                                 context_notUsed )
{
    if ( g_buttonA.isPressed() )
    {
        g_rgbLEDDriverPtr->setRgb( 255, 0, 0 );
    }
    else if ( g_buttonB.isPressed() )
    {
        //printf( "B pressed\n" );
        g_rgbLEDDriverPtr->setRgb( 0, 255, 0 );
    }
    else if ( g_buttonX.isPressed() )
    {
        //printf( "X pressed\n" );
        g_rgbLEDDriverPtr->setRgb( 0, 0, 255 );
    }
    else if ( g_buttonY.isPressed() )
    {
        //printf( "Y pressed\n" );
        g_rgbLEDDriverPtr->setRgb( 255, 255, 255 );
    }
    else
    {
        //printf( "ALL off\n" );
        g_rgbLEDDriverPtr->setOff();
    }

    // set the colour of the pen
    // parameters are red, green, blue all between 0 and 255
    graphics_.set_pen( 0, 0, 255 );

    // fill the screen with the current pen colour
    graphics_.clear();

    // draw a box to put some text in
    graphics_.set_pen( 0, 0, 0 );
    pimoroni::Rect text_rect( 10, 10, 150, 150 );
    graphics_.rectangle( text_rect );

    // write some text inside the box with 10 pixels of margin
    // automatically word wrapping
    text_rect.deflate( 10 );
    graphics_.set_pen( 255, 255, 255 );
    graphics_.text( "This is a message", pimoroni::Point( text_rect.x, text_rect.y ), text_rect.w );

    // now we've done our drawing let's update the screen
    platform_updateLcd( graphics_ );

    //st7789.update( &graphics_ );

}

/*-----------------------------------------------------------*/
//
// Set up Periodic schedulingL Core 0 
//

// Forward References
static void core0Idle( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted );
static void core0Start( Cpl::System::ElapsedTime::Precision_T currentTick );

// Periodic Intervals
static Cpl::System::PeriodicScheduler::Interval_T core0Intervals_[] =
{
    { interval_10ms, { 0, 10 }, nullptr  },
    { interval_100ms, { 0, 100 }, nullptr  },
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS
};

// Runnable object for thread
Cpl::Dm::PeriodicScheduler core0Mbox_( core0Intervals_,
                                       core0Start,
                                       nullptr,
                                       nullptr,
                                       Cpl::System::ElapsedTime::precision,
                                       core0Idle );

// In thread initialization 
void core0Start( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    // Turn the RGB LED off
    //led.set_rgb( 0, 0, 0 );

    // Start the button drivers
    g_buttonA.start();
    g_buttonB.start();
    g_buttonX.start();
    g_buttonY.start();

    platform_setLcdBacklight( 220 );
}

void core0Idle( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
}


/*-----------------------------------------------------------*/
//
// Set up Periodic schedulingL Core 1
//

// Forward References
static void core1Start( Cpl::System::ElapsedTime::Precision_T currentTick );
static void core1Idle( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted);

// Periodic Intervals
static Cpl::System::PeriodicScheduler::Interval_T core1Intervals_[] =
{
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS
};

// Runnable object for Core1 (aka the thread's 'main loop')
Cpl::Dm::PeriodicScheduler core1Mbox_( core1Intervals_,
                                       core1Start,
                                       nullptr,
                                       nullptr,
                                       Cpl::System::ElapsedTime::precision,
                                       core1Idle );

// In thread initialization 
static void core1Start( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    cmdProcessor_.getCommandProcessor().start( *g_consoleInputFdPtr, *g_consoleOutputFdPtr ); // Note: I don't need to set the 'blocking flag' because the processor knows it is non-blocking processor
}

static void core1Idle( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    cmdProcessor_.getCommandProcessor().poll();
}


/*-----------------------------------------------------------*/
void runApplication() noexcept
{
    // Enable Tracing
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( MY_APP_TRACE_SECTION );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );
    CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Hello.  I am the Circle application.") );

    // Create main application thread 
    Cpl::System::Thread::create( core0Mbox_, "APP" );

    // Create a 'Scheduler' thread for the second thread
    Cpl::System::Thread::create( core1Mbox_, "CORE2" );

    // Start scheduling
    CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Enabling scheduling") );
    Cpl::System::Api::enableScheduling();
}