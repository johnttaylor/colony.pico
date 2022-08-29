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

#include "../app.h"
#include "colony_config.h"
#include "Driver/TPipe/Maker.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Dm/PeriodicScheduler.h"
#include "Driver/Button/TPipe/Hal.h"
#include "Driver/LED/TPipe/RedGreenBlue.h"



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

// Create RGB LED Driver
static Driver::LED::TPipe::RedGreeBlue rgbLedDriver_( tpipe_, "theOne" );
Driver::LED::RedGreenBlue*             g_rgbLEDDriverPtr = &rgbLedDriver_;

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


void platform_setLcdBacklight( uint8_t value )
{
    // Not supported
}

/** TPipe Command format:

    <DD> <HH:MM:SS.sss> writeLCDData <x0> <w> <y0> <h> <hexdata>
    Where:
         <DD>                is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
         <HH:MM:SS.sss>      is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
         <x0>                Top/left X coordinate (in pixel coordinates) of the rectangle
         <w>                 Width (in display coordinates) of the rectangle. Note: <w> should always be greater than 0
         <y0>                Top/left Y coordinate (in pixel coordinates) of the rectangle
         <h>                 height (in display coordinates) of the rectangle. Note: <h> should always be greater than 0
         <hexdata>           Pixel data as 'ASCII HEX' String (upper case and with no spaces).  Each PIXEL is one byte 
                             Pixel layout is row, then column:
                                 First Pixel is:   x0, y0
                                 Pixel w is:       x0+w, y0
                                 Pixel w+1 is:     x0, y0+1
                                 Pixel (h*w) is:   x1, y1

    NOTE: Color/Pixel size RGB322 color resolution


    <DD> <HH:MM:SS.sss> updateLCD
    Where:
         <DD>                is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
         <HH:MM:SS.sss>      is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
         
    NOTE: The simulator makes a copy of the 'screen buffer' and ONLY sends 'deltas' to the simulated
          display.  This has significant positive impact on the performance of 'display' on the simulator
 */

#define NUM_DISPLAY_BYTES         (MY_APP_DISPLAY_WIDTH * MY_APP_DISPLAY_HEIGHT * sizeof(uint8_t))  
#define TPIPE_WORK_BUF_SIZE       (NUM_DISPLAY_BYTES*2 + 128)
#define DATE_SIZE_ROW             (MY_APP_DISPLAY_WIDTH * sizeof(uint8_t))

static Cpl::Text::FString<TPIPE_WORK_BUF_SIZE> buffer_;
static uint8_t                                 frameCache_[NUM_DISPLAY_BYTES];
static uint8_t*                                nextFrameCacheByte_;
static unsigned                                rowIndex_;

static void beginLCDData()
{
    nextFrameCacheByte_ = frameCache_;
    rowIndex_           = 0;
}

static void endLCDData()
{
    buffer_ = '^';
    formatMsecTimeStamp( buffer_, Cpl::System::ElapsedTime::precision().asFlatTime(), true, true );
    buffer_.formatAppend( " updateLCD" );
    buffer_ += ';';
    tpipe_.getPipeProcessor().sendRawCommand( buffer_.getString(), buffer_.length() );
}

static void sendLCDData( unsigned rowIndex, const void* data, size_t length )
{
    // Start building the TPipe command
    buffer_ = '^';
    formatMsecTimeStamp( buffer_, Cpl::System::ElapsedTime::precision().asFlatTime(), true, true );
    buffer_.formatAppend( " writeLCDData 0 %u %u 1 ",
                          MY_APP_DISPLAY_WIDTH,
                          rowIndex );

    // Add the pixel data and send the command
    Cpl::Text::bufferToAsciiHex( data, length, buffer_, true, true );
    buffer_ += ';';
    tpipe_.getPipeProcessor().sendRawCommand( buffer_.getString(), buffer_.length() );
}

static void appendLCDRowData( const void* data, size_t len )
{
    if ( memcpy( nextFrameCacheByte_, data, len ) != 0 )
    {
        memcpy( nextFrameCacheByte_, data, len );
        sendLCDData( rowIndex_, data, len );
    }

    nextFrameCacheByte_ += len;
    rowIndex_++;
}


void platform_updateLcd( pimoroni::PicoGraphics& graphics )
{
    beginLCDData();

    // Display buffer is screen native
    if ( graphics.pen_type == pimoroni::PicoGraphics::PEN_RGB332 )
    {
        const uint8_t* srcPtr = (const uint8_t*) graphics.frame_buffer;
        for ( int row=0; row < MY_APP_DISPLAY_HEIGHT; row++ )
        {
            appendLCDRowData( srcPtr, DATE_SIZE_ROW );
            srcPtr += DATE_SIZE_ROW;
        }
    }

    // Convert App's color palette to PEN_RGB332
    else
    {
        graphics.scanline_convert( pimoroni::PicoGraphics::PEN_RGB332,
                                   []( void *data, size_t length )
                                   {
                                       appendLCDRowData( data, length );
                                   }
        );
    }

    endLCDData();
}

