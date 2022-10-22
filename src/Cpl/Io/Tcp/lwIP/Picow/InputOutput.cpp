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

#include "Private_.h"
#include "Cpl/Io/Tcp/InputOutput.h"
#include "Cpl/System/FatalError.h"
#include "pico/cyw43_arch.h"

using namespace Cpl::Io::Tcp;
using namespace Cpl::Io::Tcp::lwIP::Picow;

#define PICO_LOCK   cyw43_arch_lwip_begin      
#define PICO_UNLOCK cyw43_arch_lwip_end



/////////////////////
InputOutput::InputOutput()
    : m_fd( nullptr )
    , m_eos( false )
{
}

InputOutput::InputOutput( Cpl::Io::Descriptor fd )
    : m_fd( fd )
    , m_eos( false )
{
}

InputOutput::~InputOutput( void )
{
    close();
}


///////////////////
void InputOutput::activate( Cpl::Io::Descriptor fd )
{
    Socket_T* newfd = (Socket_T *) fd.m_handlePtr;

    // Only activate if already closed 
    if ( m_fd.m_handlePtr == nullptr )
    {
        m_fd  = (void*) newfd;
        m_eos = false;
    }
    else
    {
        Cpl::System::FatalError::logf( "Cpl:Io::Tcp::InputOutput::activate().  Attempting to Activate an already opened stream." );
    }
}


///////////////////
bool InputOutput::read( void* buffer, int numBytes, int& bytesRead )
{
    Socket_T* fd = (Socket_T *) m_fd.m_handlePtr;

    // Throw an error if the socket had already been closed
    if ( fd == nullptr )
    {
        return false;
    }

    // Ignore read requests of ZERO bytes 
    if ( numBytes == 0 )
    {
        bytesRead = 0;
        return true;
    }

    // Fail if there was socket error
    bool result = false;
    PICO_LOCK();

    if ( fd->lwipPcb == nullptr )
    {
        close();
    }

    // Check if there is any received data
    else if ( fd->recvPbuf == nullptr )
    {
        // No data received -->return zero bytes read
        bytesRead = 0;
        result = true;
    }

    // Read some data...
    else
    {
        result = true;

        // Determine how much is left in the pbuf
        uint16_t remainingBytes = fd->recvPbuf->tot_len - fd->rxOffset;
        if ( numBytes > remainingBytes )
        {
            numBytes = remainingBytes;
        }

        // Copy the data to the caller's buffer
        bytesRead = pbuf_copy_partial( fd->recvPbuf, buffer, numBytes, fd->rxOffset );
        tcp_recved( fd->lwipPcb, bytesRead );

        // Consumed all of the data -->free the buffer
        if ( fd->recvPbuf->tot_len == bytesRead + fd->rxOffset )
        {
            pbuf_free( fd->recvPbuf );
            fd->rxOffset = 0;
            fd->recvPbuf = nullptr;
        }

        // Adjust the offset to begin at the start of the 'unread data'
        else
        {
            fd->rxOffset += bytesRead;
        }
    }

    PICO_UNLOCK();
    return result;
}

bool InputOutput::available()
{
    Socket_T* fd = (Socket_T *) m_fd.m_handlePtr;

    PICO_LOCK();
    bool haveData = fd->recvPbuf != nullptr;
    PICO_UNLOCK();

    return haveData;
}


//////////////////////
bool InputOutput::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    Socket_T* fd = (Socket_T *) m_fd.m_handlePtr;

    // Throw an error if the socket had already been closed
    if ( fd == nullptr )
    {
        return false;
    }

    // Ignore write requests of ZERO bytes 
    if ( maxBytes == 0 )
    {
        bytesWritten = 0;
        return true;
    }

    // Fail if there was socket error
    PICO_LOCK();
    if ( fd->lwipPcb == nullptr )
    {
        close();
        PICO_UNLOCK();
        return false;
    }

    // Check if there is any outgoing buffer space
    uint16_t availLen = tcp_sndbuf( fd->lwipPcb );
    if ( availLen == ERR_MEM || availLen == 0 )
    {
        bytesWritten = 0;
        PICO_UNLOCK();
        return true;
    }

    // Adjust how many bytes can be sent
    if ( maxBytes > availLen )
    {
        maxBytes = availLen;
    }

    // Send the data
    err_t err = tcp_write( fd->lwipPcb, buffer, maxBytes, TCP_WRITE_FLAG_COPY );
    if ( err )
    {
        // If there is out of memory error -->wait for something to free up
        if ( err == ERR_MEM )
        {
            bytesWritten = 0;
        }

        // Unrecoverable error
        else
        {
            close();
            PICO_UNLOCK();
            return false;
        }
    }

    bytesWritten = maxBytes;
    PICO_UNLOCK();
    return true;
}

void InputOutput::flush()
{
    Socket_T* fd = (Socket_T *) m_fd.m_handlePtr;
    if ( fd != nullptr && fd->lwipPcb )
    {
        PICO_LOCK();
        tcp_output( fd->lwipPcb );
        PICO_UNLOCK();
    }
}

bool InputOutput::isEos()
{
    PICO_LOCK();
    bool eos = m_eos;
    PICO_UNLOCK();
    return eos;
}

void InputOutput::close()
{
    Socket_T* fd = (Socket_T *) m_fd.m_handlePtr;

    PICO_LOCK();

    // Throw an error if the socket had already been closed
    if ( fd != nullptr && fd->lwipPcb )
    {
        // Make sure any unprocessed PBUF gets freed
        if ( fd->recvPbuf )
        {
            pbuf_free( fd->recvPbuf );
            fd->recvPbuf = nullptr;
        }

        tcp_close( fd->lwipPcb );
        fd->lwipPcb      = nullptr;
        m_fd.m_handlePtr = nullptr;
    }

    m_eos = true;
    PICO_UNLOCK();
}
