#ifndef app_h
#define app_h
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
/** @file

    This file defines the top level interface(s) to the platform independent
    application.

 */

#include "libraries/pico_graphics/pico_graphics.hpp"
#include "Driver/Button/PolledDebounced.h"
#include "Driver/LED/RedGreenBlue.h"


/// Width, in pixels, of the display
#define MY_APP_DISPLAY_WIDTH        240

/// Height, in pixels, of the display
#define MY_APP_DISPLAY_HEIGHT       135

/// Default Trace section
#define MY_APP_TRACE_SECTION        "app"

/// expose the Button A driver
extern Driver::Button::PolledDebounced g_buttonA;

/// expose the Button B driver
extern Driver::Button::PolledDebounced g_buttonB;

/// expose the Button X driver
extern Driver::Button::PolledDebounced g_buttonX;

/// expose the Button Y driver
extern Driver::Button::PolledDebounced g_buttonY;

/// expose the RGB LED driver
extern Driver::LED::RedGreenBlue*      g_rgbLEDDriverPtr;

/// This function is called by the Application to update/refresh the LCD contents
void platform_updateLcd( pimoroni::PicoGraphics& graphics );

/// This function is called by the Application to set the LCD backlight setting (0-255)
void platform_setLcdBacklight( uint8_t value );

/// Entry function to the application
void runApplication() noexcept;


#endif // end header latch
