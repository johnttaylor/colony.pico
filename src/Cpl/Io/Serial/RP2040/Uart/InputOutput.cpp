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

#include "InputOutput.h"
#include <hardware/gpio.h>
#include <hardware/irq.h>
#include <hardware/regs/intctrl.h>

///
using namespace Cpl::Io::Serial::RP2040::Uart;

InputOutput* InputOutput::m_uart0Instance;
InputOutput* InputOutput::m_uart1Instance;


////////////////////////////////////
InputOutput::InputOutput( uint8_t       memTxBuffer[],
                          size_t        txBufSize,
                          uint8_t       memRxBuffer[],
                          size_t        rxBufSize,
                          uart_inst_t*  uartHdl,
                          unsigned long baudRate,
                          unsigned      txPin,
                          unsigned      rxPin
)
    : m_uartHdl( uartHdl )
    , m_txBuffer( memTxBuffer )
    , m_txBufSize( txBufSize )
    , m_txHead( memTxBuffer )
    , m_txTail( memTxBuffer )
    , m_rxBuffer( memRxBuffer )
    , m_rxBufSize( rxBufSize )
    , m_rxHead( memRxBuffer )
    , m_rxTail( memRxBuffer )
{
    // Initialize the hardware
    uart_init( uartHdl, baudRate );
    uart_set_format( uartHdl, 8, 1, UART_PARITY_NONE );
    gpio_set_function( txPin, GPIO_FUNC_UART );
    gpio_set_function( rxPin, GPIO_FUNC_UART );
    uart_set_fifo_enabled( uartHdl, true );

    // Set up IRQ Handlers
    if ( uartHdl == CPL_IO_SERIAL_RP2040_UART_HANDLE_UART0 )
    {
        m_uart0Instance = this;
        irq_clear( UART0_IRQ );
        irq_set_exclusive_handler( UART0_IRQ, uart0IrqHandler );
        irq_set_enabled( UART0_IRQ, true );
        configureInterrupts( true, OPTION_BSP_DEFAULT_UART0_RX_FIFO_IRQ_THRESHOLD, OPTION_BSP_DEFAULT_UART0_TX_FIFO_IRQ_THRESHOLD );
    }
    else if ( uartHdl == CPL_IO_SERIAL_RP2040_UART_HANDLE_UART1 )
    {
        m_uart1Instance = this;
        irq_clear( UART1_IRQ );
        irq_set_exclusive_handler( UART1_IRQ, uart1IrqHandler );
        irq_set_enabled( UART1_IRQ, true );
        configureInterrupts( true, OPTION_BSP_DEFAULT_UART1_RX_FIFO_IRQ_THRESHOLD, OPTION_BSP_DEFAULT_UART1_TX_FIFO_IRQ_THRESHOLD );
    }
}



InputOutput::~InputOutput( void )
{
    // Disable the RX/TX interrupts
    configureInterrupts( false, OPTION_BSP_DEFAULT_UART0_RX_FIFO_IRQ_THRESHOLD, OPTION_BSP_DEFAULT_UART0_TX_FIFO_IRQ_THRESHOLD );

    if ( m_uartHdl == CPL_IO_SERIAL_RP2040_UART_HANDLE_UART0 )
    {
        irq_set_enabled( UART0_IRQ, false );
        irq_remove_handler( UART0_IRQ, uart0IrqHandler );
        m_uart0Instance = 0;
    }
    else if ( uartHdl == CPL_IO_SERIAL_RP2040_UART_HANDLE_UART1 )
    {
        irq_set_enabled( UART1_IRQ, false );
        irq_remove_handler( UART1_IRQ, _uart1_IRQ_handler );
        m_uart1Instance = 0;
    }

    uart_deinit( m_uartHdl );
}


////////////////////////////////////
void InputOutput::uart0IrqHandler( void )
{
    if ( m_uart0Instance )
    {
        m_uart0Instance->irqHandler();
    }
}

/// ISR handler for UART0
void InputOutput::uart1IrqHandler( void )
{
    if ( m_uart1Instance )
    {
        m_uart1Instance->irqHandler();
    }
}

void InputOutput::irqHandler()
{
    // NOTE: The assumption is made that reading/writing the Tail/Head pointers  
    //       are atomic operations per the ARM architecture

    // Get the IRQ flags
    uint32_t mis = uart_get_hw( m_uartHdl )->mis;

    // Receive IRQ
    if ( mis & (UART_UARTMIS_RXMIS_BITS | UART_UARTMIS_RTMIS_BITS) )
    {

        // Drain the HW Receive FIFO
        while ( uart_is_readable( m_uartHdl ) )
        {
            // Read the incoming byte
            uint8_t c = (uint8_t) uart_getc( m_uartHdl );

            // Calculate a new head pointer (handling rolling over at the end of the buffer space)
            uint8_t* newRxHead = (uint8_t*) (m_rxHead + 1);
            if ( (newRxHead - m_rxBuffer) >= m_rxBufSize )
            {
                newRxHead = m_rxBuffer;
            }

            // Discard the incoming byte if the SW FIFO is full
            if ( newRxHead != m_rxTail )
            {
                *m_rxHead = c;
                m_rxHead  = newRxHead;
            }
        }
    }

    // Transmit IRQ
    if ( mis & (UART_UARTMIS_TXMIS_BITS) )
    {
        // Fill the HW Transmit FIFO
        while ( uart_is_writable( m_uartHdl ) && m_txTail != m_txHead )
        {
            // Write the outgoing byte
            uart_putc_raw( m_uartHdl, (char) *m_txTail );

            // Calculate a new tail pointer (handling rolling over at the end of the buffer space)
            uint8_t* newTxTail = (uint8_t*) (m_txTail + 1);
            if ( (newTxTail - m_txBuffer) >= m_txBufSize )
            {
                newTxTail = m_txBuffer;
            }
            m_txTail = newTxTail;
        }

        // Disable TX IRQ for now if we have nothing left to transmit
        if ( m_txTail == m_txHead )
        {
            uart_get_hw( m_uartHdl )->imsc = (UART_UARTIMSC_RXIM_BITS | UART_UARTIMSC_RTIM_BITS);
        }
    }
}


////////////////////////////////////
bool InputOutput::read( void* buffer, int numBytes, int& bytesRead )
{
    // NOTE: The assumption is made that reading/writing the Tail/Head pointers  
    //       are atomic operations per the ARM architecture

    size_t  bytesIn = 0;
    uint8_t* dstPtr = (uint8_t*) buffer;

    // Drain the SW RX FIFO
    while ( numBytes && m_rxHead != m_rxTail )
    {
        // Get the next available byte
        *dstPtr++ = *m_rxTail;
        numBytes--;
        bytesRead++;

        // Calculate a new tail pointer (handling rolling over at the end of the buffer space)
        uint8_t* newRxTail = (uint8_t*) (m_rxTail + 1);
        if ( (newRxTail - m_rxBuffer) >= m_rxBufSize )
        {
            newRxTail = m_rxBuffer;
        }
        m_rxTail = newRxTail;

    }

    return true;
}

bool InputOutput::available()
{
    return m_rxHead != m_rxTail;
}


////////////////////////////////////
bool InputOutput::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    bytesWritten = maxBytes;
    return m_tx.write( buffer, (size_t) maxBytes );
}


void InputOutput::flush()
{
    // Not supported/has no meaning for a serial port
}

bool InputOutput::isEos()
{
    // Does not really have meaning for serial port (assuming the serial port is opened/active)
    return false;
}

void InputOutput::close()
{
    m_tx.stop();
    m_rx.stop();
}

//////////////////////////////
void InputOutput::configureInterrupts( bool enabled, unsigned rxFifoThreshold, unsigned txFifoThreshold )
{
    if ( !enabled )
    {
        uart_get_hw( m_uartHdl )->imsc = ~(UART_UARTIMSC_TXIM_BITS | UART_UARTIMSC_RXIM_BITS | UART_UARTIMSC_RTIM_BITS);
    }
    else
    {
        uart_get_hw( m_uartHdl )->imsc = (UART_UARTIMSC_RXIM_BITS | UART_UARTIMSC_RTIM_BITS);
        hw_write_masked( &uart_get_hw( m_uartHdl )->ifls, rxFifoThreshold << UART_UARTIFLS_RXIFLSEL_LSB, UART_UARTIFLS_RXIFLSEL_BITS );
        hw_write_masked( &uart_get_hw( m_uartHdl )->ifls, txFifoThreshold << UART_UARTIFLS_TXIFLSEL_LSB, UART_UARTIFLS_TXIFLSEL_BITS );
    }
}

