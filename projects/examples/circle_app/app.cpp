#include "app.h"
//#include "drivers/st7789/st7789.hpp"
//#include "rgbled.hpp"
//#include "button.hpp"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Dm/PeriodicScheduler.h"
#include "Driver/Button/PolledDebounced.h"

//using namespace pimoroni;

// Display driver
//ST7789 st7789( PicoDisplay2::WIDTH, PicoDisplay2::HEIGHT, ROTATE_0, false, get_spi_pins( BG_SPI_FRONT ) );

// Graphics library - in RGB332 mode you get 256 colours and optional dithering for 75K RAM.
//PicoGraphics_PenRGB332 graphics( st7789.width, st7789.height, nullptr );

// RGB LED
//pimoroni::RGBLED led( 6, 7, 8 );


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

// Graphics library - in RGB332 mode you get 256 colours and optional dithering for 75K RAM.
pimoroni::PicoGraphics_PenRGB332 graphics( 240, 135, nullptr );

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
    graphics.set_pen( 30, 40, 50 );

    // fill the screen with the current pen colour
    graphics.clear();

    // draw a box to put some text in
    graphics.set_pen( 10, 20, 30 );
    pimoroni::Rect text_rect( 10, 10, 150, 150 );
    graphics.rectangle( text_rect );

    // write some text inside the box with 10 pixels of margin
    // automatically word wrapping
    text_rect.deflate( 10 );
    graphics.set_pen( 110, 120, 130 );
    graphics.text( "This is a message", pimoroni::Point( text_rect.x, text_rect.y ), text_rect.w );

    // now we've done our drawing let's update the screen
    platform_updateLcd( graphics );

    //st7789.update( &graphics );

}

/*-----------------------------------------------------------*/
//
// Set up Periodic scheduling 
//

static Cpl::System::PeriodicScheduler::Interval_T core0Intervals_[] =
{
    { interval_10ms, { 0, 10 }, nullptr  },
    { interval_100ms, { 0, 100 }, nullptr  },
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS
};

// Forward Reference
static void core0Start( Cpl::System::ElapsedTime::Precision_T currentTick );

// Runnable object for Core0 (aka the thread's 'main loop')
Cpl::Dm::PeriodicScheduler core0Mbox_( core0Intervals_, core0Start );

// In thread initialization for code the executes on Core 0
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


/*-----------------------------------------------------------*/
void runApplication() noexcept
{
    // Enable Tracing
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( MY_APP_TRACE_SECTION );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );
    CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Hello.  I am the Circle application.") );

    // Create mock application thread 
    Cpl::System::Thread::create( core0Mbox_, "APP" );

    // Create a 'Scheduler' thread for the TShell to run in
    //Cpl::System::Thread::create( core1Mbox_, "TSHELL" );

    // Start scheduling
    CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Enabling scheduling") );
    Cpl::System::Api::enableScheduling();
}