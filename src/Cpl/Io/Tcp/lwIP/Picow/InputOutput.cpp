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

#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#define SECT_ "Cpl::Io::Tcp::lwIP::Picow"

///
using namespace Cpl::Io::Tcp;
using namespace Cpl::Io::Tcp::lwIP::Picow;


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
        CPL_SYSTEM_TRACE_MSG( SECT_, ("read. Stream closed" ) );
        return false;
    }

    // Ignore read requests of ZERO bytes 
    if ( numBytes == 0 )
    {
        bytesRead = 0;
        return true;
    }

    // Fail if there was socket error
    cyw43_arch_lwip_begin();
    if ( fd->lwipPcb == nullptr )
    {
        close();
        cyw43_arch_lwip_end();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("read. fd->lwipPcb is NULL") );
        return false;
    }

    // Check if there is any received data
    if ( fd->recvPbuf == nullptr )
    {
        // No data received -->return zero bytes read
        bytesRead = 0;
        cyw43_arch_lwip_end();
        return true;
    }

    // Copy the data to the caller's buffer
    bytesRead = pbuf_copy_partial( fd->recvPbuf, buffer, numBytes, fd->rxOffset );
    tcp_recved( fd->lwipPcb, bytesRead );

    // Consumed all of the data -->free the buffer
    if ( fd->recvPbuf->tot_len == bytesRead + fd->rxOffset )
    {
        fd->rxOffset = 0;
        fd->recvPbuf = nullptr;
        pbuf_free( fd->recvPbuf );
    }

    // Adjust the offset to begin at the start of the 'unread data'
    else
    {
        fd->rxOffset += bytesRead;
    }
    
    cyw43_arch_lwip_end();
    return true;
}

bool InputOutput::available()
{
    Socket_T* fd = (Socket_T *) m_fd.m_handlePtr;

    cyw43_arch_lwip_begin();
    bool haveData = fd->recvPbuf != nullptr;
    cyw43_arch_lwip_end();

    return haveData;
}


//////////////////////
bool InputOutput::write( const void* buffer, int maxBytes, int& bytesWritten )
{
    Socket_T* fd = (Socket_T *) m_fd.m_handlePtr;

    // Throw an error if the socket had already been closed
    if ( fd == nullptr )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("write. Stream closed") );
        return false;
    }

    // Ignore read requests of ZERO bytes 
    if ( maxBytes == 0 )
    {
        bytesWritten = 0;
        return true;
    }

    // Fail if there was socket error
    cyw43_arch_lwip_begin();
    if ( fd->lwipPcb == nullptr )
    {
        close();
        cyw43_arch_lwip_end();
        CPL_SYSTEM_TRACE_MSG( SECT_, ("write. fd->lwipPcb is NULL") );
        return false;
    }

    // Check if there is any outgoing buffer space
    uint16_t availLen = tcp_sndbuf( fd->lwipPcb );
    if ( availLen == ERR_MEM )
    {
        bytesWritten = 0;
        return true;
    }
    CPL_SYSTEM_TRACE_MSG( SECT_, ("tcp_write....  aval=%d, maxBytes=%d", availLen, maxBytes) );

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
            cyw43_arch_lwip_end();
            CPL_SYSTEM_TRACE_MSG( SECT_, ("tcp_write. failed. err=%d", err) );
            return false;
        }
    }
    else
    {
        //tcp_output( fd->lwipPcb );
    }
    bytesWritten = maxBytes;
    cyw43_arch_lwip_end();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("tcp_write: out [%.*s]", bytesWritten, buffer) );

    return true;
}

void InputOutput::flush()
{
    Socket_T* fd = (Socket_T *) m_fd.m_handlePtr;
    if ( fd != nullptr && fd->lwipPcb )
    {
        cyw43_arch_lwip_begin();
        tcp_output( fd->lwipPcb );
        cyw43_arch_lwip_end();
    }
}

bool InputOutput::isEos()
{
    cyw43_arch_lwip_begin();
    bool eos = m_eos;
    cyw43_arch_lwip_end();
    return eos;
}

void InputOutput::close()
{
    Socket_T* fd = (Socket_T *) m_fd.m_handlePtr;

    cyw43_arch_lwip_begin();

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
    cyw43_arch_lwip_end();
}
