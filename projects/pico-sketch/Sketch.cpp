/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "app.h"
#include "Sketch.h"
#include "colony_config.h"
#include "ModelPoints.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Dm/MailboxServer.h"

static void drawStartScreen();
static void clearTheCanvas( uint8_t bgColorRed, uint8_t bgColorGreen, uint8_t bgColorBlue );

/*-----------------------------------------------------------*/
class EventMonitor
{
public:
    /// Constructor
    EventMonitor( Cpl::Dm::MailboxServer&  myMbox )
        : m_obAltButton( myMbox, *this, &EventMonitor::buttonEventAlt )
        , m_obLeftButton( myMbox, *this, &EventMonitor::buttonEventLeft )
        , m_obRightButton( myMbox, *this, &EventMonitor::buttonEventRight )
        , m_obUpButton( myMbox, *this, &EventMonitor::buttonEventUp )
        , m_obDownButton( myMbox, *this, &EventMonitor::buttonEventDown )
        , m_obSizeButton( myMbox, *this, &EventMonitor::buttonEventSize )
        , m_obColorButton( myMbox, *this, &EventMonitor::buttonEventColor )
        , m_obClearButton( myMbox, *this, &EventMonitor::buttonEventClear)
    {}

public:
    /// Initialize
    void start()
    {
        // Housekeeping
        m_waitingAnyKey = true;

        // Subscribe for change notifications
        mp::buttonEventAlt.attach( m_obAltButton );
        mp::buttonEventLeft.attach( m_obLeftButton );
        mp::buttonEventRight.attach( m_obRightButton );
        mp::buttonEventUp.attach( m_obUpButton );
        mp::buttonEventDown.attach( m_obDownButton );
        mp::buttonEventSize.attach( m_obSizeButton );
        mp::buttonEventColor.attach( m_obColorButton );
        mp::buttonEventClear.attach( m_obClearButton );
    }


public:
    /// Button event
    void buttonEventAlt( Cpl::Dm::Mp::Bool& mp )
    {
        printf( "buttonEventAlt: valid=%d\n", !mp.isNotValid() );
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
        }
    }
    
    /// Button event
    void buttonEventRight( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
        }
    }

    /// Button event
    void buttonEventLeft( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
        }
    }

    /// Button event
    void buttonEventUp( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
        }
    }

    /// Button event
    void buttonEventDown( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
        }
    }

    /// Button event
    void buttonEventSize( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
        }
    }

    /// Button event
    void buttonEventColor( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
        }
    }

    /// Button event
    void buttonEventClear( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
        }
    }

public:
    /// Helper function to the first keypess when on the start screen
    bool consumeAnyKeyWhenWaiting()
    {
        printf( "consumeAnyKeyWhenWaiting: m_waitingAnyKey=%d\n", m_waitingAnyKey );

        if ( m_waitingAnyKey )
        {
            clearTheCanvas( 255, 255, 255 );
            m_waitingAnyKey = false;
            return true;
        }
        return false;
    }

public:
    /// Observers
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obAltButton;
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obLeftButton;
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obRightButton;
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obUpButton;
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obDownButton;
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obSizeButton;
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obColorButton;
    Cpl::Dm::SubscriberComposer<EventMonitor, Cpl::Dm::Mp::Bool> m_obClearButton;

    /// Waiting for any kee
    bool m_waitingAnyKey;
};


/*---------------------------------------------------------------------------*/

static EventMonitor events_( *g_uiRunnablePtr );

void intializeUI()
{
    // Turn the RGB LED off
    g_rgbLEDDriverPtr->setOff();

    // Draw start/splash screen
    drawStartScreen();

    // Start the Event monitor
    events_.start();
}

void processUI( Cpl::System::ElapsedTime::Precision_T currentTick,
                Cpl::System::ElapsedTime::Precision_T currentInterval )
{

}

///////////////////////////////////////////////////////////////////////////////

//
// SCREEN OPERATIONS
//

// Graphics library: Use RGB332 mode (256 colours) on the Target to limit RAM usage canvas 
#ifndef USE_PICO_SKETCH_RGB565
pimoroni::PicoGraphics_PenRGB332 graphics_( MY_APP_DISPLAY_WIDTH, MY_APP_DISPLAY_HEIGHT, nullptr );

// Graphics library: Use RGB565 mode (64K colours) on the Simulator because RAM is NOT an issue
#else
pimoroni::PicoGraphics_PenRGB565 graphics_( MY_APP_DISPLAY_WIDTH, MY_APP_DISPLAY_HEIGHT, nullptr );
#endif



/*---------------------------------------------------------------------------*/
void clearTheCanvas( uint8_t bgColorRed, uint8_t bgColorGreen, uint8_t bgColorBlue )
{
    // set the colour of the pen
    graphics_.set_pen( bgColorRed, bgColorGreen, bgColorBlue );

    // fill the screen with the current pen colour
    graphics_.clear();

    // now we've done our drawing let's update the screen
    platform_updateLcd( graphics_ );
}

/*---------------------------------------------------------------------------*/
#define X0_TITLE        40
#define Y0_TITLE        0

#define X0_HELP_LINE                18
#define COLOFFSET_HELP_LINE_COL2    68
#define COLOFFSET_HELP_LINE_COL3    (COLOFFSET_HELP_LINE_COL2+38)
#define COLOFFSET_HELP_LINE_COL4    (COLOFFSET_HELP_LINE_COL3+60)
#define HEIGHT_HELP_LINES           22
#define Y0_HELP_LINE1               42
#define Y0_HELP_LINE2               (Y0_HELP_LINE1+HEIGHT_HELP_LINES)
#define Y0_HELP_LINE3               (Y0_HELP_LINE2+HEIGHT_HELP_LINES)
#define Y0_HELP_LINE4               (Y0_HELP_LINE3+HEIGHT_HELP_LINES)
#define WRAP_HELP_LINES 205

#define HELP_TEXT_LINE1_COL1        "Right:"
#define HELP_TEXT_LINE1_COL2        "X"
#define HELP_TEXT_LINE1_COL3        "Left:"
#define HELP_TEXT_LINE1_COL4        "A+X"

#define HELP_TEXT_LINE2_COL1        "Down:"
#define HELP_TEXT_LINE2_COL2        "Y"
#define HELP_TEXT_LINE2_COL3        "Up:"
#define HELP_TEXT_LINE2_COL4        "A+Y"

#define HELP_TEXT_LINE3_COL1        "Color:"
#define HELP_TEXT_LINE3_COL2        "B"
#define HELP_TEXT_LINE3_COL3        "Size:"
#define HELP_TEXT_LINE3_COL4        "A+B"

#define HELP_TEXT_LINE4_COL1        "Cursor:"
#define HELP_TEXT_LINE4_COL2        "A"
#define HELP_TEXT_LINE4_COL3        "Clear:"
#define HELP_TEXT_LINE4_COL4        "A+X+Y"

void drawStartScreen()
{
    // set the colour of the pen
    graphics_.set_pen( 0, 0, 255 );

    // fill the screen with the current pen colour
    graphics_.clear();

    // Title
    graphics_.set_pen( 0, 255, 0 );
    graphics_.set_font( &font14_outline );
    graphics_.text( "Pico-Sketch", pimoroni::Point( X0_TITLE, Y0_TITLE ), 240 );

    // Help
    graphics_.set_pen( 255, 255, 255 );
    graphics_.set_font( &font8 );
    graphics_.text( HELP_TEXT_LINE1_COL1, pimoroni::Point( X0_HELP_LINE, Y0_HELP_LINE1 ), 240 );
    graphics_.text( HELP_TEXT_LINE1_COL2, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL2, Y0_HELP_LINE1 ), 240 );
    graphics_.text( HELP_TEXT_LINE1_COL3, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL3, Y0_HELP_LINE1 ), 240 );
    graphics_.text( HELP_TEXT_LINE1_COL4, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL4, Y0_HELP_LINE1 ), 240 );

    graphics_.text( HELP_TEXT_LINE2_COL1, pimoroni::Point( X0_HELP_LINE, Y0_HELP_LINE2 ), 240 );
    graphics_.text( HELP_TEXT_LINE2_COL2, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL2, Y0_HELP_LINE2 ), 240 );
    graphics_.text( HELP_TEXT_LINE2_COL3, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL3, Y0_HELP_LINE2 ), 240 );
    graphics_.text( HELP_TEXT_LINE2_COL4, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL4, Y0_HELP_LINE2 ), 240 );
 
    graphics_.text( HELP_TEXT_LINE3_COL1, pimoroni::Point( X0_HELP_LINE, Y0_HELP_LINE3 ), 240 );
    graphics_.text( HELP_TEXT_LINE3_COL2, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL2, Y0_HELP_LINE3 ), 240 );
    graphics_.text( HELP_TEXT_LINE3_COL3, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL3, Y0_HELP_LINE3 ), 240 );
    graphics_.text( HELP_TEXT_LINE3_COL4, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL4, Y0_HELP_LINE3 ), 240 );

    graphics_.text( HELP_TEXT_LINE4_COL1, pimoroni::Point( X0_HELP_LINE, Y0_HELP_LINE4 ), 240 );
    graphics_.text( HELP_TEXT_LINE4_COL2, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL2, Y0_HELP_LINE4 ), 240 );
    graphics_.text( HELP_TEXT_LINE4_COL3, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL3, Y0_HELP_LINE4 ), 240 );
    graphics_.text( HELP_TEXT_LINE4_COL4, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL4, Y0_HELP_LINE4 ), 240 );


    // now we've done our drawing let's update the screen
    platform_updateLcd( graphics_ );
}