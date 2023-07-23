#ifndef Driver_DIO_RP2040_mapping_h_
#define Driver_DIO_RP2040_mapping_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2023  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "hardware/gpio.h"

/// Structure that defines the a Pin
struct DriverDioRP2040OutPinConfig_T
{
    uint                pinNum;         //!< Pin number
    gpio_drive_strength driveStrength;  //!< Drive strength
    bool                pullUp;         //!< Enables the internal pull up
    bool                pullDown;       //!< Enables the internal pull down
};

/// Platform mapping
#define Driver_Dio_Out_PinConfig_T_MAP      DriverDioRP2040OutPinConfig_T


/*--------------------------------------------------------------------------*/
#endif  // end header latch
