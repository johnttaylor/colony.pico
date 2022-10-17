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

#include "Cpl/System/Trace.h"
#define SECT_ "_0test"

err_t Cpl::Io::Tcp::lwIP::Picow::lwipCb_dataSent_( void* arg, struct tcp_pcb* pcb, uint16_t len )
{
    //CPL_SYSTEM_TRACE_MSG( SECT_, ("lwipCb_dataSent_: len=%d", len) );
    // Not used/needed
    return ERR_OK;
}


err_t Cpl::Io::Tcp::lwIP::Picow::lwipCb_dataReceived_( void* arg, struct tcp_pcb* pcb, struct pbuf* pbuf, err_t err )
{
    Socket_T* fd = (Socket_T *) arg;
    //CPL_SYSTEM_TRACE_MSG( SECT_, ("lwipCb_dataReceived_: err=%d, len=%d", err, pbuf->tot_len) );

    // Fail if no valid upper layer 
    if ( fd == nullptr )
    {
        tcp_abort( pcb );
        return ERR_ABRT;
    }

    // Fail if mismatch in PCB instances OR TCP error 
    if ( fd->lwipPcb != pcb || err != ERR_OK )
    {
        if ( fd->recvPbuf )
        {
            pbuf_free( fd->recvPbuf );
        }
        fd->recvPbuf = nullptr;
        fd->lwipPcb  = nullptr;
        tcp_abort( pcb );
        return ERR_ABRT;
    }

    // If we haven't processed the last PBUF - add to the current pbuf
    if ( fd->recvPbuf != nullptr )
    {
        pbuf_cat( fd->recvPbuf, pbuf );
    }
    
    // Cache the incoming PBUF/receive data so the next read call will process it
    else
    {
        fd->recvPbuf = pbuf;
    }
    return ERR_OK;
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


