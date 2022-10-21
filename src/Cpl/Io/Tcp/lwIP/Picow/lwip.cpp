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

#include "Cpl/Io/Tcp/lwIP/Picow/Private_.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Private_.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>

#include "Cpl/System/Trace.h"
#define SECT_ "_0test"

#define PICO_LOCK()
#define PICO_UNLOCK()


//#define PICO_LOCK   cyw43_arch_lwip_begin      
//#define PICO_UNLOCK cyw43_arch_lwip_end

//#include "Cpl/System/GlobalLock.h"
//#define PICO_LOCK   Cpl::System::GlobalLock::begin      
//#define PICO_UNLOCK Cpl::System::GlobalLock::end


err_t Cpl::Io::Tcp::lwIP::Picow::lwipCb_dataSent_( void* arg, struct tcp_pcb* pcb, uint16_t len )
{
    //CPL_SYSTEM_TRACE_MSG( SECT_, ("lwipCb_dataSent_: len=%d", len) );
    // Not used/needed
    return ERR_OK;
}

err_t Cpl::Io::Tcp::lwIP::Picow::lwipCb_poll_( void* arg, struct tcp_pcb* pcb )
{
    Socket_T* fd = (Socket_T *) arg;

    // Flush any pending output
    if ( fd != nullptr && fd->lwipPcb == pcb )
    {
        err_t err =  tcp_output( pcb );
        if ( err )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("lwipCb_poll_: tcp_output(), err=%d", err) );
        }
        return err;

    }
    return ERR_OK;
}

err_t Cpl::Io::Tcp::lwIP::Picow::lwipCb_dataReceived_( void* arg, struct tcp_pcb* pcb, struct pbuf* pbuf, err_t err )
{
    Socket_T* fd     = (Socket_T *) arg;
    err_t     result = ERR_OK;

    PICO_LOCK();

    // Fail if no valid upper layer 
    if ( fd == nullptr )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("lwipCb_dataReceived_: fd == nullptr") );
        tcp_abort( pcb );
        result = ERR_ABRT;
    }

    // Trap additional failures
    else if ( pcb == nullptr || err != ERR_OK || fd->lwipPcb != pcb )
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("lwipCb_dataReceived_: fd->lwipPcb (%p) != pcb (%p)", fd->lwipPcb, pcb) );
        if ( fd->recvPbuf )
        {
            pbuf_free( fd->recvPbuf );
        }
        if ( pbuf != nullptr )
        {
            pbuf_free( pbuf );
        }

        fd->recvPbuf = nullptr;
        fd->lwipPcb  = nullptr;
        tcp_abort( pcb );
        result = ERR_ABRT;
    }

    // If we haven't processed the last PBUF - add to the current pbuf
    else if ( pbuf )
    {
        if ( fd->recvPbuf != nullptr )
        {
            // Account for overflowing the max pbuf size
            uint32_t newLen = fd->recvPbuf->tot_len + pbuf->tot_len;
            if ( newLen > 0xFFFF )
            {
                result = ERR_WOULDBLOCK;
            }
            else
            {
                // Append the new data to my current pbuf
                pbuf_cat( fd->recvPbuf, pbuf );
            }
        }

        // Cache the incoming PBUF/receive data so the next read call will process it
        else
        {
            fd->recvPbuf = pbuf;
        }
    }
    CPL_SYSTEM_TRACE_MSG( SECT_, ("lwipCb_dataReceived_: err=%d, len=%d, curLen=%d, off=%u, unread=%u", err, pbuf->tot_len, fd->recvPbuf->tot_len, fd->rxOffset, fd->recvPbuf->tot_len - fd->rxOffset) );
    
    PICO_UNLOCK();
    return result;
}

void Cpl::Io::Tcp::lwIP::Picow::lwipCb_error_( void* arg, err_t err )
{
    Socket_T* fd = (Socket_T *) arg;
    CPL_SYSTEM_TRACE_MSG( SECT_, ("lwipCb_error_: err=%d", err) );

    if ( fd != nullptr )
    {
        // TODO: Does pbuf_free() need to be called on: fd->recvPbuf ?
        fd->lwipPcb  = nullptr;
        fd->recvPbuf = nullptr;
    }
}


