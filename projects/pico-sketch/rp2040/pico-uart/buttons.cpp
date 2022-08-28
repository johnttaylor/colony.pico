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

#include "../../app.h"
#include "Driver/Button/RP2040/Hal.h"
#include "main.h"


#define APP_BUTTON_A_PIN_NUM    12
#define APP_BUTTON_B_PIN_NUM    13
#define APP_BUTTON_X_PIN_NUM    14
#define APP_BUTTON_Y_PIN_NUM    15

Driver::Button::PolledDebounced g_buttonA( { APP_BUTTON_A_PIN_NUM, true } );
Driver::Button::PolledDebounced g_buttonB( { APP_BUTTON_B_PIN_NUM, true } );
Driver::Button::PolledDebounced g_buttonX( { APP_BUTTON_X_PIN_NUM, true } );
Driver::Button::PolledDebounced g_buttonY( { APP_BUTTON_Y_PIN_NUM, true } );


void platform_initialize_buttons()
{
    driverButtonHalRP2040_initialize( g_buttonA.getHandle() );
    driverButtonHalRP2040_initialize( g_buttonB.getHandle() );
    driverButtonHalRP2040_initialize( g_buttonX.getHandle() );
    driverButtonHalRP2040_initialize( g_buttonY.getHandle() );
}
    