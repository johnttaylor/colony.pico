/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Cpl/System/Trace.h"
#include "Cpl/Io/Serial/RP2040/Uart/InputOutput.h"


// Size, in bytes, of the Software TX fifo for the 'Console' where trace message will be routed
#ifndef OPTION_CPL_SYSTEM_RP2040_CONSOLE_TX_FIFO_SIZE    
#define OPTION_CPL_SYSTEM_RP2040_CONSOLE_TX_FIFO_SIZE    256
#endif

#define RX_FIFO_SIZE    512

static uint8_t txFIFO_[TX_FIFO_SIZE];
static uint8_t rxFIFO_[RX_FIFO_SIZE];

static Cpl::Io::Serial::RP2040::Uart::InputOutput uartfd_( txFIFO_, sizeof( txFIFO_ ), rxFIFO_, sizeof( rxFIFO_ ) );

/// 
using namespace Cpl::System;

///
static Cpl::Io::Stdio::StdOut fd_;


////////////////////////////////////////////////////////////////////////////////
Cpl::Io::Output* Trace::getDefaultOutputStream_( void ) noexcept
{
    return &fd_;
}

