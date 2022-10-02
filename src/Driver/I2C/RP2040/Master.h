#ifndef Driver_I2C_RP2040_Master_h_
#define Driver_I2C_RP2040_Master_h_
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


#include "Driver/I2C/Master.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

///
namespace Driver {
///
namespace I2C {
///
namespace RP2040 {


/** This class implements the I2C interface for the Raspberry PI RP2040
    micro-controller using the low level I2C calls defined in the Pico C/C++
    SDK.

    The interface itself is NOT thread safe. It is the responsibility of
    the users/clients of the driver to handle any threading issues.
 */
class Master : public Driver::I2C::Master
{
public:
    /** Constructor.

        The 'i2cInstance' argument must be either 'i2c0' or 'i2c1'

        The application is responsible for configuring GPIO pins used by the
        'i2cInstance' PRIOR to calling start().  The configI2CPins() method is
        an optional helper function available to the application for configuring
        the I2C pins.
     */
    Master( i2c_inst_t*   i2cInstance,
            unsigned long baudrate  = 100 * 1000,   // 100KHz
            unsigned long timeoutMs = 1000 );       // 1 second

public:
    /** Helper method to configure the I2C Pins.  See the constructor
        comments for additional details.
     */
    inline static void configureI2CPins( unsigned sdaPin = PICO_DEFAULT_I2C_SDA_PIN,
                                         unsigned sclPin = PICO_DEFAULT_I2C_SCL_PIN )
    {
        gpio_set_function( sdaPin, GPIO_FUNC_I2C );
        gpio_set_function( sclPin, GPIO_FUNC_I2C );
        gpio_pull_up( sdaPin );
        gpio_pull_up( sclPin );
    }

public:
    /// See Driver::I2C::Master
    bool start() noexcept;

    /// See Driver::I2C::Master
    void stop() noexcept;

    /// See Driver::I2C::Master
    Result_T  writeToDevice( uint8_t     device7BitAddress,
                             unsigned    numBytesToTransmit,
                             const void* srcData,
                             bool        noStop = false ) noexcept;

    /// See Driver::I2C::Master
    Result_T readFromDevice( uint8_t   device7BitAddress,
                             unsigned  numBytesToRead,
                             void*     dstData,
                             bool      noStop = false );


    /// See Driver::I2C::Master
    unsigned long setBaudRate( unsigned long newBaudRateHz ) noexcept;

    /// See Driver::I2C::Master
    unsigned long setTransactionTimeout( unsigned long maxTimeMs ) noexcept;

protected:
    /// Handle the low-level SDK driver instance
    i2c_inst_t*     m_i2cDevice;

    /// The current I2C baud rate
    unsigned long   m_baudrate;

    /// The current transaction timeout value
    unsigned long   m_timeoutMs;

    /// Track my started state
    bool            m_started;

};




};      // end namespaces
};
};
#endif  // end header latch
