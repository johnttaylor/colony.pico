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

#include "Driver/PicoDisplay/Api.h" // Must be first #include (because of the Pimoroni/Pico SDK)
#include "Ui.h"
#include "Storm/Thermostat/ModelPoints.h"
#include "Storm/Thermostat/Ui/ModelPoints.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Dm/SubscriberComposer.h"
#include "Cpl/Dm/MailboxServer.h"
#include "Cpl/System/Trace.h"



/*-----------------------------------------------------------*/
/** This class monitors event sources (okay just the logical buttons
    at this point) and updates the UI appropriately 
 */
class Events
{

public:
    /// Constructor
    Events( Cpl::Dm::MailboxServer&  myMbox )
        : m_obAltButton( myMbox, *this, &EventMonitor::buttonEventAlt )
        , m_obLeftButton( myMbox, *this, &EventMonitor::buttonEventLeft )
        , m_obRightButton( myMbox, *this, &EventMonitor::buttonEventRight )
        , m_obUpButton( myMbox, *this, &EventMonitor::buttonEventUp )
        , m_obDownButton( myMbox, *this, &EventMonitor::buttonEventDown )
        , m_obSizeButton( myMbox, *this, &EventMonitor::buttonEventSize )
        , m_obColorButton( myMbox, *this, &EventMonitor::buttonEventColor )
        , m_obClearButton( myMbox, *this, &EventMonitor::buttonEventClear )
    {}

public:
    /// Initialize
    void start()
    {
        // Housekeeping
        m_waitingAnyKey = true;
        m_sizeIndex     = 0;
        m_colorIndex    = 0;
        m_cursorActive  = false;

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
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
            toggleCursor();
        }
    }

    /// Button event
    void buttonEventRight( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
            deactivateCursor();
            movePencil( 1, 0 );
        }
    }

    /// Button event
    void buttonEventLeft( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
            deactivateCursor();
            movePencil( -1, 0 );
        }
    }

    /// Button event
    void buttonEventUp( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
            deactivateCursor();
            movePencil( 0, -1 );
        }
    }

    /// Button event
    void buttonEventDown( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
            deactivateCursor();
            movePencil( 0, 1 );
        }
    }

    /// Button event
    void buttonEventSize( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {

            deactivateCursor();

            const static uint8_t sizes[] ={ 1, 2, 3, 4, 6, 8, 10, 20 };
            m_sizeIndex++;
            if ( m_sizeIndex >= sizeof( sizes ) )
            {
                m_sizeIndex = 0;
            }
            setPencilSize( sizes[m_sizeIndex] );

            activateCursor();
        }
    }

    /// Button event
    void buttonEventColor( Cpl::Dm::Mp::Bool& mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
            // Colors: Black, Blue, Green, Cyan, Red, Magenta, Yellow, White }
            const static RGB_T colors[] ={ {0,0,0},         // Black
                                           {0,0,255},       // Blue
                                           {0,255,0},       // Green
                                           {0,255,255},     // Cyan
                                           {255,0,0},       // Red
                                           {255,0,255},     // Magenta
                                           {255,255,0},    // Yellow
                                           {255,255,255}   // White
            };
            m_colorIndex++;
            if ( m_colorIndex >= (sizeof( colors ) / sizeof( RGB_T )) )
            {
                m_colorIndex = 0;
            }

            setPencilColor( colors[m_colorIndex].red, colors[m_colorIndex].green, colors[m_colorIndex].blue );
            Driver::PicoDisplay::Api::rgbLED().setRgb( colors[m_colorIndex].red, colors[m_colorIndex].green, colors[m_colorIndex].blue );
        }
    }

    /// Button event
    void buttonEventClear( Cpl::Dm::Mp::Bool & mp )
    {
        bool pressed;
        if ( mp.read( pressed ) && pressed && !consumeAnyKeyWhenWaiting() )
        {
            drawStartScreen();
            m_waitingAnyKey = true;
            m_sizeIndex     = 0;
            m_colorIndex    = 0;
            m_cursorActive  = false;
        }
    }

public:
    /// Helper function to the first key pess when on the start screen
    bool consumeAnyKeyWhenWaiting()
    {
        if ( m_waitingAnyKey )
        {
            clearTheCanvas( 255, 255, 255 );
            m_waitingAnyKey = false;
            return true;
        }
        return false;
    }

    /// Helper method to deactivates the cursor (if active)
    void deactivateCursor()
    {
        if ( m_cursorActive )
        {
            m_cursorActive = false;
            restoreCursorBlock();
        }
    }

    /// Helper method to activate the cursor
    void activateCursor()
    {
        if ( !m_cursorActive )
        {
            m_cursorActive = true;
            showCursor();
        }
    }

    /// Helper method to toggle the cursor
    void toggleCursor()
    {
        if ( !m_cursorActive )
        {
            m_cursorActive = true;
            showCursor();
        }
        else
        {
            m_cursorActive = false;
            restoreCursorBlock();
        }
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

    /// Current size index
    unsigned    m_sizeIndex;

    /// Current color index
    unsigned    m_colorIndex;

    /// Waiting for any key
    bool        m_waitingAnyKey;

    /// State of the Cursor
    bool        m_cursorActive;
};


/*---------------------------------------------------------------------------*/

static EventMonitor events_( *g_uiRunnablePtr );
#endif

void Storm::Thermostat::Ui::PicoDisplay::intializeUI()
{
    // Populate my working variables
    mp_userMode.read( uiMode_ );
    mp_fanMode.read( uiFan_ );

    // Draw start/splash screen
    drawStartScreen();

    // Start the Event monitor
    //events_.start();
}

void Storm::Thermostat::Ui::PicoDisplay::processUI( Cpl::System::ElapsedTime::Precision_T currentTick,
                                                    Cpl::System::ElapsedTime::Precision_T currentInterval )
{

}

void Storm::Thermostat::Ui::PicoDisplay::shutdownUI()
{
    //events_.stop();
}

///////////////////////////////////////////////////////////////////////////////

//
// SCREEN OPERATIONS
//

// Graphics library: Use RGB332 mode (256 colours) on the Target to limit RAM usage canvas 
pimoroni::PicoGraphics_PenRGB332 graphics_( OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH, OPTION_DRIVER_PICO_DISPLAY_LCD_HEIGHT, nullptr );


/*---------------------------------------------------------------------------*/
/*
   Mode: Heat  Fan: Auto
   Room: 78.5  Out: -23.4
   Cool: 79   Heat: 60

   G:100% Y1 Y2 W1 W2 W3

   Heat Mode   Fan Cont
   Room: 78.5  Cool: 79
   Out: -23.5  Heat: 70

   G:100% Y1 Y2 W1 W2 W3
*/

#define BACKGROUND_COLOR_R          0
#define BACKGROUND_COLOR_G          0
#define BACKGROUND_COLOR_B          0

#define FOREGROUND_TEXT_COLOR_R     255
#define FOREGROUND_TEXT_COLOR_G     255
#define FOREGROUND_TEXT_COLOR_B     255

#define TEXT_HEIGHT                 18

#define COLUMN0_X0                  10
#define COLUMN1_X0                  120

#define MODE_LABEL_COLOR_R          FOREGROUND_TEXT_COLOR_R    
#define MODE_LABEL_COLOR_G          FOREGROUND_TEXT_COLOR_G
#define MODE_LABEL_COLOR_B          FOREGROUND_TEXT_COLOR_B
#define MODE_LABEL_X0               COLUMN0_X0
#define MODE_LABEL_Y0               12
#define MODE_LABEL_WIDTH            50

#define MODE_NOMINAL_COLOR_R        255    
#define MODE_NOMINAL_COLOR_G        255
#define MODE_NOMINAL_COLOR_B        0
#define MODE_ACTIVE_COLOR_R         255    
#define MODE_ACTIVE_COLOR_G         255
#define MODE_ACTIVE_COLOR_B         0
#define MODE_BKGRD_ACTIVE_COLOR_R   0    
#define MODE_BKGRD_ACTIVE_COLOR_G   0
#define MODE_BKGRD_ACTIVE_COLOR_B   255
#define MODE_X0                     (MODE_LABEL_X0+MODE_LABEL_WIDTH+4)
#define MODE_Y0                     MODE_LABEL_Y0
#define MODE_WIDTH                  43
#define MODE_HEIGHT                 TEXT_HEIGHT
#define MODE_OUTLINE_X0             (MODE_X0 - MODE_OUTLINE_DELTA)
#define MODE_OUTLINE_Y0             (MODE_Y0 - MODE_OUTLINE_DELTA)
#define MODE_OUTLINE_WIDTH          (MODE_WIDTH + MODE_OUTLINE_DELTA)
#define MODE_OUTLINE_HEIGHT         (MODE_HEIGHT + MODE_OUTLINE_DELTA-1)
#define MODE_OUTLINE_DELTA          4

#define FAN_LABEL_COLOR_R          FOREGROUND_TEXT_COLOR_R    
#define FAN_LABEL_COLOR_G          FOREGROUND_TEXT_COLOR_G
#define FAN_LABEL_COLOR_B          FOREGROUND_TEXT_COLOR_B
#define FAN_LABEL_X0               COLUMN1_X0
#define FAN_LABEL_Y0               MODE_LABEL_Y0
#define FAN_LABEL_WIDTH            40

#define FAN_NOMINAL_COLOR_R        MODE_NOMINAL_COLOR_R    
#define FAN_NOMINAL_COLOR_G        MODE_NOMINAL_COLOR_G
#define FAN_NOMINAL_COLOR_B        MODE_NOMINAL_COLOR_B
#define FAN_ACTIVE_COLOR_R         MODE_ACTIVE_COLOR_R    
#define FAN_ACTIVE_COLOR_G         MODE_ACTIVE_COLOR_G
#define FAN_ACTIVE_COLOR_B         MODE_ACTIVE_COLOR_B
#define FAN_BKGRD_ACTIVE_COLOR_R   MODE_BKGRD_ACTIVE_COLOR_R    
#define FAN_BKGRD_ACTIVE_COLOR_G   MODE_BKGRD_ACTIVE_COLOR_G
#define FAN_BKGRD_ACTIVE_COLOR_B   MODE_BKGRD_ACTIVE_COLOR_B
#define FAN_X0                     (FAN_LABEL_X0+FAN_LABEL_WIDTH+4)
#define FAN_Y0                     FAN_LABEL_Y0
#define FAN_WIDTH                  43
#define FAN_HEIGHT                 TEXT_HEIGHT
#define FAN_OUTLINE_X0             (FAN_X0 - FAN_OUTLINE_DELTA)
#define FAN_OUTLINE_Y0             (FAN_Y0 - FAN_OUTLINE_DELTA)
#define FAN_OUTLINE_WIDTH          (FAN_WIDTH + FAN_OUTLINE_DELTA)
#define FAN_OUTLINE_HEIGHT         (FAN_HEIGHT + FAN_OUTLINE_DELTA-1)
#define FAN_OUTLINE_DELTA          4

void drawMode( bool active )
{
    pimoroni::Rect box( MODE_OUTLINE_X0, MODE_OUTLINE_Y0, MODE_OUTLINE_WIDTH, MODE_OUTLINE_HEIGHT );

    // nominal display (i.e. not being updated)
    if ( !active )
    {
        graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
        graphics_.rectangle( box );
        graphics_.set_pen( MODE_NOMINAL_COLOR_R, MODE_NOMINAL_COLOR_G, MODE_NOMINAL_COLOR_B );
    }

    // Active/being-updated
    else
    {
        graphics_.set_pen( MODE_BKGRD_ACTIVE_COLOR_R, MODE_BKGRD_ACTIVE_COLOR_G, MODE_BKGRD_ACTIVE_COLOR_B );
        graphics_.rectangle( box );
        graphics_.set_pen( MODE_ACTIVE_COLOR_R, MODE_ACTIVE_COLOR_G, MODE_ACTIVE_COLOR_B );
    }

    const char* val = "OFF";
    switch ( uiMode_ )
    {
    case Storm::Type::ThermostatMode::eAUTO: val = "AUTO"; break;
    case Storm::Type::ThermostatMode::eCOOLING: val = "COOL"; break;
    case Storm::Type::ThermostatMode::eHEATING: val = "HEAT"; break;
    case Storm::Type::ThermostatMode::eID_HEATING: val = "IHEAT"; break;
    default:
        break;
    }
    graphics_.text( val, pimoroni::Point( MODE_X0, MODE_Y0 ), OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH );
}

void drawFan( bool active )
{
    pimoroni::Rect box( FAN_OUTLINE_X0, FAN_OUTLINE_Y0, FAN_OUTLINE_WIDTH, FAN_OUTLINE_HEIGHT );

    // nominal display (i.e. not being updated)
    if ( !active )
    {
        graphics_.set_pen( BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B );
        graphics_.rectangle( box );
        graphics_.set_pen( FAN_NOMINAL_COLOR_R, FAN_NOMINAL_COLOR_G, FAN_NOMINAL_COLOR_B );
    }

    // Active/being-updated
    else
    {
        graphics_.set_pen( FAN_BKGRD_ACTIVE_COLOR_R, FAN_BKGRD_ACTIVE_COLOR_G, FAN_BKGRD_ACTIVE_COLOR_B );
        graphics_.rectangle( box );
        graphics_.set_pen( FAN_ACTIVE_COLOR_R, FAN_ACTIVE_COLOR_G, FAN_ACTIVE_COLOR_B );
    }

    const char* val = "AUTO";
    if ( uiFan_ == +Storm::Type::FanMode::eCONTINUOUS )
    {
        val = "CONT";
    }
    graphics_.text( val, pimoroni::Point( FAN_X0, FAN_Y0 ), OPTION_DRIVER_PICO_DISPLAY_LCD_WIDTH );
}

void drawStartScreen()
{
    // Turn the RGB LED off
    Driver::PicoDisplay::Api::rgbLED().setOff();
    Driver::PicoDisplay::Api::rgbLED().setBrightness( 64 );

    // set the colour of the pen
    graphics_.set_pen( 0, 0, 0 );

    // fill the screen with the current pen colour
    graphics_.clear();


    // Help
    graphics_.set_font( &font8 );
    graphics_.set_pen( MODE_LABEL_COLOR_R, MODE_LABEL_COLOR_G, MODE_LABEL_COLOR_B );
    graphics_.text( "mode:", pimoroni::Point( MODE_LABEL_X0, MODE_LABEL_Y0 ), 240 );
    drawMode( false );
    graphics_.set_pen( FAN_LABEL_COLOR_R, FAN_LABEL_COLOR_G, FAN_LABEL_COLOR_B );
    graphics_.text( "fan:", pimoroni::Point( FAN_LABEL_X0, FAN_LABEL_Y0 ), 240 );
    drawFan( false );
    //pimoroni::Rect box( X0_MODE_LINE + 50, Y0_MODE_LINE, 40,  20 );
    ////graphics_.set_pen( 255, 255, 0 );
    ////graphics_.rectangle( box );
    //graphics_.set_pen( 255, 255, 0 );
    ////graphics_.set_pen( 0, 0, 0 );
    //graphics_.text( "HEAT", pimoroni::Point( X0_MODE_LINE + 50 +2 , Y0_MODE_LINE + 2), 240 );

//    graphics_.text( "Room: 78.5  Out: -23.4", pimoroni::Point( X0_MODE_LINE, Y0_TEMPS_LINE ), 240 );
    //graphics_.set_pen( 255, 255, 0 );
    //graphics_.text( "cool: 79     heat: 60", pimoroni::Point( X0_MODE_LINE, Y0_SETPTS_LINE ), 240 );
    //graphics_.set_pen( 0, 255, 255 );
    //graphics_.text( "G:100%   Y1 Y2   W1 W2 W3", pimoroni::Point( X0_MODE_LINE, Y0_OUTPUTS_LINE ), 240 );

    /*   graphics_.text( HELP_TEXT_LINE1_COL1, pimoroni::Point( X0_HELP_LINE, Y0_HELP_LINE1 ), 240 );
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
       graphics_.text( HELP_TEXT_LINE4_COL4, pimoroni::Point( X0_HELP_LINE + COLOFFSET_HELP_LINE_COL4, Y0_HELP_LINE4 ), 240 );*/


       // now we've done our drawing let's update the screen
    Driver::PicoDisplay::Api::updateLCD( graphics_ );
}

