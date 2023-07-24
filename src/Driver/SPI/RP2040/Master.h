#ifndef Driver_SPI_RP2040_Master_h_
#define Driver_SPI_RP2040_Master_h_
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


#include "Driver/SPI/Master.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

///
namespace Driver {
///
namespace SPI {
///
namespace RP2040 {


/** This class implements the SPI interface for the Raspberry PI RP2040
    micro-controller using the low level SPI calls defined in the Pico C/C++
    SDK.
 */
class Master : public Driver::SPI::Master
{
public:
    /** Constructor.

        The 'spiInstance' argument must be either 'spi0' or 'spi1'

        The application is responsible for configuring GPIO pins used by the
        'spiInstance' PRIOR to calling start().  The configSPIPins() method is
        an optional helper function available to the application for configuring
        the SPI pins.
     */
    Master( spi_inst_t* spiInstance,
            unsigned    sclk = PICO_DEFAULT_SPI_SCK_PIN,
            unsigned    mosi = PICO_DEFAULT_SPI_TX_PIN,
            unsigned    miso = PICO_DEFAULT_SPI_RX_PIN,
            size_t      baudrate  = 8 * 1000 * 1000, // 8MHz
            uint        dataBits  = 8,               // 8bits, 
            spi_cpol_t  cpol      = SPI_CPOL_0,      // Clock Polarity = clock high 
            spi_cpha_t  cpha      = SPI_CPHA_0 );    // Clock Phase = data transmission begins on the falling edge of the slave select signal


public:
    /// See Driver::SPI::Master
    bool start( size_t newBaudRateHz = 0 ) noexcept;

    /// See Driver::SPI::Master
    void stop() noexcept;

    /// See Driver::SPI::Master
    bool  transfer( size_t      numBytes,
                    const void* srcData,
                    void*       dstData = nullptr ) noexcept;

protected:

    /// Handle the low-level SDK driver instance
    spi_inst_t*         m_spiDevice;

    /// The current SPI baud rate
    size_t              m_baudrate;

    /// Number of data bits
    uint                m_dataBits;
    
    /// Clock polarity
    spi_cpol_t          m_cpol;

    /// Clock Phase
    spi_cpha_t          m_cpha;

    /// Track my started state
    bool                m_started;
};




};      // end namespaces
};
};
#endif  // end header latch
