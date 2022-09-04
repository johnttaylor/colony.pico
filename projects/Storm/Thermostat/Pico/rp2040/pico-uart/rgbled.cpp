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
#include "Driver/LED/PimoroniPico/RedGreenBlue.h"


#define APP_LED_RGB_R       6
#define APP_LED_RGB_G       7
#define APP_LED_RGB_B       8

static Driver::LED::PimoroniPico::RedGreenBlue rgbLEDDriver_( APP_LED_RGB_R , APP_LED_RGB_G, APP_LED_RGB_B );
Driver::LED::RedGreenBlue*                     g_rgbLEDDriverPtr = &rgbLEDDriver_;

