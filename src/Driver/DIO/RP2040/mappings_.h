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
#include "hardware/pwm.h"
#include <string.h>

/// Structure that defines an Output Pin
struct DriverDioOutRP2040PinConfig_T
{
    uint                pinNum;         //!< Pin number
    gpio_drive_strength driveStrength;  //!< Drive strength
    bool                pullUp;         //!< Enables the internal pull up
    bool                pullDown;       //!< Enables the internal pull down

    /// Constructor
    DriverDioOutRP2040PinConfig_T( uint pinNumber, 
                                   gpio_drive_strength strength=GPIO_DRIVE_STRENGTH_2MA,
                                   bool pullUpRestistor = false,
                                   bool pullDownRestistor = false )
        : pinNum( pinNumber )
        , driveStrength( strength )
        , pullUp( pullUpRestistor )
        , pullDown( pullDownRestistor )
    {
    }

    /// Copy Constructor
    DriverDioOutRP2040PinConfig_T( const DriverDioOutRP2040PinConfig_T& other )
        : pinNum( other.pinNum )
        , driveStrength( other.driveStrength )
        , pullUp( other.pullUp )
        , pullDown( other.pullDown )
    {
    }
};

/** Structure that defines an PWM signal & Pin
    TODO: Currently only supports:
        - PWM config is free-running at system clock speed
        - no phase correction
        - wrapping at 0xffff, 
        - with standard polarities for channels A and B.
 */
struct DriverDioPwmRP2040Config_T
{
    uint16_t                        wrap;       //!< Set the PWM Counter wrap value
    DriverDioOutRP2040PinConfig_T   pin;        //!< Configuration for the PIN

    /// Constructor
    DriverDioPwmRP2040Config_T( uint pinNumber, uint16_t wrapDefault=0xFFFF )
        : wrap( wrapDefault )
        , pin( pinNumber )
    {
    }

    /// Copy Constructor
    DriverDioPwmRP2040Config_T( const DriverDioPwmRP2040Config_T& other )
        : wrap( other.wrap )
        , pin( other.pin )
    {
    }
};

/// Platform mapping
#define DriverDioOutPinConfig_T_MAP         DriverDioOutRP2040PinConfig_T

/// Platform mapping
#define DriverDioPwmConfig_T_MAP            DriverDioPwmRP2040Config_T

/*--------------------------------------------------------------------------*/
#endif  // end header latch
