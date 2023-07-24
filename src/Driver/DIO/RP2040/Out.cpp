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

#include "Driver/DIO/Out.h"

using namespace Driver::DIO;

    
Out::Out( const DriverDioOutPinConfig_T& pinConfig, bool assertedHigh )
    : m_pin( pinConfig )
    , m_assertedHigh( assertedHigh )
    , m_started( false )
{
}
bool Out::start()
{
    if ( !m_started )
    {
        m_started = true;
        gpio_init( m_pin.pinNum );
        gpio_set_dir( m_pin.pinNum, GPIO_OUT );
        gpio_set_drive_strength( m_pin.pinNum, m_pin.driveStrength );
        gpio_set_pulls( m_pin.pinNum, m_pin.pullUp, m_pin.pullDown );
        return true;
    }

    return false;
}

void Out::stop()
{
    if ( m_started )
    {
        m_started = false;
        gpio_deinit( m_pin.pinNum );
    }
}

bool Out::getOutput() const
{
    if ( m_started )
    {
        bool phy = gpio_get_out_level( m_pin.pinNum );
        return m_assertedHigh ? phy : !phy;
    }

    return false;
}

void Out::setOutput( bool asserted )
{
    if ( m_started )
    {
        gpio_put( m_pin.pinNum, m_assertedHigh ? asserted : !asserted );
    }
}


