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

using namespace Driver::SPI::RP2040;

//////////////////////////////////////////////////////////////////////////////
Master::Master( spi_inst_t* spiInstance,
                size_t      baudrate,
                uint        dataBits,
                spi_cpol_t  cpol,
                spi_cpha_t  cpha )
    : m_spiDevice( spiInstance )
    , m_baudrate( baudrate )
    , m_dataBits( dataBits )
    , m_cpol( cpol )
    , m_cpha( cpha )
    , m_started( false )
{
    CPL_SYSTEM_ASSERT( spiInstance == spi0 || spiInstance == spi1 );
}

//////////////////////////////////////////////////////////////////////////////
bool Master::start( size_t newBaudRateHz ) noexcept
{
    if ( !m_started )
    {
        m_started = true;
        spi_init( newBaudRateHz ? newBaudRateHz : m_baudrate );
        spi_set_format( m_spiDevice, m_dataBits, m_cpol, m_cpha, SPI_MSB_FIRST );
        return true;
    }

    return false;
}

void Master::stop() noexcept
{
    if ( m_started )
    {
        m_started = false;
        spi_deinit( m_spiDevice );
    }
}


bool  Master::transfer( size_t      numBytes,
                        const void* srcData,
                        void*       dstData ) noexcept
{
    int r = 0;
    if ( dstData == nullptr )
    {
        r = spi_write_blocking( m_spiDevice, (const uint8_t*) srcData, numBytes );
    }
    else
    {
        r = spi_write_read_blocking( m_spiDevice, (const uint8_t*) srcData, (uint8_t*) dstData, numBytes );
    }

    return r == numBytes;
}


