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

#include "AsyncListener.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/Trace.h"
#include "pico/cyw43_arch.h"

#define SECT_ "Cpl::Io::Tcp::lwIP::Picow"

using namespace Cpl::Io::Tcp::lwIP::Picow;



///////////////////////////////////////////////////
AsyncListener::AsyncListener()
    : m_listenerPcb( nullptr )
    , m_clientPtr( nullptr )
{
}

AsyncListener::~AsyncListener()
{
    terminate();
}

///////////////////////////////////////////////////
bool AsyncListener::startListening( Client& client,
                                    int     portNumToListenOn ) noexcept
{
    // Fail if already 'started'
    if ( m_clientPtr == nullptr )
    {
        m_clientPtr = &client;

        cyw43_arch_lwip_begin();

        struct tcp_pcb* pcb = tcp_new_ip_type( IPADDR_TYPE_ANY );
        if ( !pcb )
        {
            cyw43_arch_lwip_end();
            CPL_SYSTEM_TRACE_MSG( SECT_, ("AsyncListener: failed to create listen pcb") );
            return false;
        }

        err_t err = tcp_bind( pcb, NULL, portNumToListenOn );
        if ( err )
        {
            cyw43_arch_lwip_end();
            CPL_SYSTEM_TRACE_MSG( SECT_, ("AsyncListener: failed to bind to port %d (err=%d)", portNumToListenOn, err) );
            return false;
        }

        // Start listening
        m_listenerPcb = tcp_listen_with_backlog( pcb, 1 );
        if ( !m_listenerPcb )
        {
            if ( pcb )
            {
                tcp_close( pcb );
            }
            cyw43_arch_lwip_end();
            CPL_SYSTEM_TRACE_MSG( SECT_, ("AsyncListener: failed to listen") );
            return false;
        }

        // Set the callback function for accept function
        tcp_arg( m_listenerPcb, this );
        tcp_accept( m_listenerPcb, lwIPCb_accept );

        cyw43_arch_lwip_end();
        return true;
    }

    return false;
}

err_t AsyncListener::lwIPCb_accept( void* arg, struct tcp_pcb* newpcb, err_t err )
{
    AsyncListener* theOne = (AsyncListener*) arg;

    // Trap errors
    if ( theOne == nullptr || err != ERR_OK || newpcb == nullptr )
    {
        return ERR_VAL;
    }

    // Reject the request if there is already an active connection
    if ( theOne->m_connectionFd.lwipPcb != nullptr )
    {
        tcp_abort( newpcb );
        return ERR_ABRT;
    }

    // Set up the new 'file descriptor' for the connection
    memset( &(theOne->m_connectionFd), 0, sizeof( Socket_T ) );
    theOne->m_connectionFd.lwipPcb = newpcb;
    Cpl::Io::Descriptor newfd      = (void*) &(theOne->m_connectionFd);

    // Set the callback function for the connection
    tcp_arg( newpcb, &(theOne->m_connectionFd) );
    tcp_sent( newpcb, lwipCb_dataSent_ );
    tcp_recv( newpcb, lwipCb_dataReceived_ );
    tcp_err( newpcb, lwipCb_error_ );

    // Inform the client of new connection
    if ( !theOne->m_clientPtr->newConnection( newfd, ip4addr_ntoa( &(newpcb->remote_ip) ) ) )
    {
        // Housekeeping
        memset( &(theOne->m_connectionFd), 0, sizeof( Socket_T ) );

        // Client rejected the connection
        tcp_abort( newpcb );
        return ERR_ABRT;
    }

    return ERR_OK;
}

void AsyncListener::poll() noexcept
{
    // Not used/needed
}

void AsyncListener::terminate() noexcept
{
    // Guard LWIP calls
    cyw43_arch_lwip_begin();
    
    // Close listener PCB
    if ( m_listenerPcb != nullptr )
    {
        tcp_close( m_listenerPcb );
        m_listenerPcb = nullptr;
    }

    // Terminate the accepted connection 
    if ( m_clientPtr )
    {
        m_clientPtr->close();
    }
    
    // Clear my 'started state'
    m_clientPtr = nullptr;

    cyw43_arch_lwip_begin();
}



