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

#include "Driver/DIO/Pwm.h"

using namespace Driver::DIO;

    
Pwm::Pwm( const DriverDioPwmConfig_T& pwmConfig )
    : m_pwm( pwmConfig )
    , m_started( false )

{
}
bool Pwm::start( size_t initialDutyCycle )
{
    if ( !m_started )
    {
        m_started = true;
        // Pin config
        gpio_set_drive_strength( m_pwm.pin.pinNum, m_pwm.pin.driveStrength );
        gpio_set_pulls( m_pwm.pin.pinNum, m_pwm.pin.pullUp, m_pwm.pin.pullDown );
        gpio_set_function( m_pwm.pin.pinNum, GPIO_FUNC_PWM );

        // PWM config
        uint       sliceNum  = pwm_gpio_to_slice_num( m_pwm.pin.pinNum );
        pwm_config cfg       = pwm_get_default_config();
        pwm_init( sliceNum, &cfg, false );
        pwm_set_wrap( sliceNum, m_pwm.wrap );
        pwm_set_gpio_level( m_pwm.pin.pinNum, initialDutyCycle );
        pwm_set_enabled( sliceNum, true );
        return true;
    }

    return false;
}

void Pwm::stop()
{
    if ( m_started )
    {
        m_started = false;
        uint sliceNum  = pwm_gpio_to_slice_num( m_pwm.pin.pinNum );
        pwm_set_enabled( sliceNum, false );
        gpio_deinit( m_pwm.pin.pinNum );
    }
}

size_t Pwm::getMaxDutyCycle() const
{
    return m_pwm.wrap;
}

void Pwm::setDutyCycle( size_t dutyCycle )
{
    if ( m_started )
    {
        pwm_set_gpio_level( m_pwm.pin.pinNum, dutyCycle );
    }
}


