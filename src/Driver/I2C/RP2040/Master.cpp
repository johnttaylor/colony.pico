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
/** @file */

#include "Master.h"
#include "Cpl/System/Assert.h"
#include "pico/time.h"

using namespace Driver::I2C::RP2040;

//////////////////////////////////////////////////////////////////////////////
Master::Master( i2c_inst_t*   i2cInstance,
                unsigned long baudrate,
                unsigned long timeoutMs )
    : m_i2cDevice( i2cInstance )
    , m_baudrate( baudrate )
    , m_timeoutMs( timeoutMs )
    , m_started( false )
{
    CPL_SYSTEM_ASSERT( i2cInstance == i2c0 || i2cInstance == i2c1 );
}

//////////////////////////////////////////////////////////////////////////////
bool Master::start() noexcept
{
    if ( !m_started )
    {
        m_started = true;
        setBaudRate( m_baudrate );
        return true;
    }

    return false;
}

void Master::stop() noexcept
{
    if ( m_started )
    {
        m_started = false;
        i2c_deinit( m_i2cDevice );
    }
}


Driver::I2C::Master::Result_T Master::writeToDevice( uint8_t     device7BitAddress,
                                                     unsigned    numBytesToTransmit,
                                                     const void* srcData,
                                                     bool        noStop ) noexcept
{
    // Fail if not started
    if ( !m_started )
    {
        return eNOT_STARTED;
    }

    int ret = i2c_write_blocking_until( m_i2cDevice,
                                        device7BitAddress,
                                        (const uint8_t*) srcData,
                                        numBytesToTransmit,
                                        noStop,
                                        make_timeout_time_ms( (uint32_t) m_timeoutMs ) );

    return ret == (int) numBytesToTransmit ? eSUCCESS : ret == PICO_ERROR_GENERIC ? eNO_ACK : eTIMEOUT;
}

Driver::I2C::Master::Result_T Master::readFromDevice( uint8_t   device7BitAddress,
                                                      unsigned  numBytesToRead,
                                                      void*     dstData,
                                                      bool      noStop )
{
    // Fail if not started
    if ( !m_started )
    {
        return eNOT_STARTED;
    }

    int ret = i2c_read_blocking_until( m_i2cDevice,
                                       device7BitAddress,
                                       (uint8_t*) dstData,
                                       numBytesToRead,
                                       noStop,
                                       make_timeout_time_ms( (uint32_t) m_timeoutMs ) );

    return ret == (int) numBytesToRead ? eSUCCESS : ret == PICO_ERROR_GENERIC ? eNO_ACK : eTIMEOUT;
}


unsigned long Master::setBaudRate( unsigned long newBaudRateHz ) noexcept
{
    unsigned long prevBaud = m_baudrate;
    m_baudrate = newBaudRateHz;
    i2c_init( m_i2cDevice, m_baudrate );
    return prevBaud;
}

unsigned long Master::setTransactionTimeout( unsigned long maxTimeMs ) noexcept
{
    unsigned long prevTimeout = m_timeoutMs;
    m_timeoutMs = maxTimeMs;
    return prevTimeout;
}