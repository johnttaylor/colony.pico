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

#include "AsyncConnector.h"
#include "Private_.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"
#include "Cpl/System/Trace.h"

using namespace Cpl::Io::Tcp::lwIP::Picow;
#define SECT_ "Cpl::Io::Tcp::lwIP::Picow"

#ifdef PICO_CYW43_ARCH_THREADSAFE_BACKGROUND
#define PICO_LOCK       cyw43_arch_lwip_begin      
#define PICO_UNLOCK     cyw43_arch_lwip_end
#define PICO_CB_LOCK()
#define PICO_CB_UNLOCK()

#elif PICO_CYW43_ARCH_POLL
#define PICO_LOCK       g_internalLock.lock      
#define PICO_UNLOCK     g_internalLock.unlock
#define PICO_CB_LOCK    g_internalLock.lock    
#define PICO_CB_UNLOCK  g_internalLock.unlock

#else
#error MUST be compiled with: PICO_CYW43_ARCH_POLL=1 or PICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1
#endif


/////////////////////////////////////////////
AsyncConnector::AsyncConnector()
    : m_connectorPcb( nullptr )
    , m_clientPtr( nullptr )
{
}

AsyncConnector::~AsyncConnector()
{
    terminate();
}

bool AsyncConnector::establish( Client&     client,
                                const char* remoteHostName,
                                int         portNumToConnectTo )
{
    bool result = false;
    PICO_LOCK();

    // Fail if already 'started'
    if ( m_clientPtr == nullptr )
    {
        // Housekeeping
        memset( &m_connectionFd, 0, sizeof( Socket_T ) );
        ip4addr_aton( remoteHostName, &m_remoteAddr );
        m_clientPtr                        = &client;
        m_connectionFd.connnectorClientPtr = &client;
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Connecting to %s port %u", ip4addr_ntoa( &m_remoteAddr ), portNumToConnectTo) );

        // Create socket
        m_connectorPcb = tcp_new_ip_type( IP_GET_TYPE( &m_remoteAddr ) );
        if ( m_connectorPcb == nullptr )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("AsyncConnector: Failed to create PCB") );
        }


        // Socket was successfully created
        else
        {
            // Set the callback functions for the connection
            tcp_arg( m_connectorPcb, &m_connectionFd );
            tcp_poll( m_connectorPcb, lwipCb_poll_, OPTION_CPL_IO_TCP_LWIP_PICOW_POLL_TICKS );
            tcp_sent( m_connectorPcb, lwipCb_dataSent_ );
            tcp_recv( m_connectorPcb, lwipCb_dataReceived_ );
            tcp_err( m_connectorPcb, lwipCb_error_ );

            // Initiate the connection sequence
            err_t err =  tcp_connect( m_connectorPcb, &m_remoteAddr, portNumToConnectTo, lwIPCb_connected );
            if ( err == ERR_OK )
            {
                result = true;
            }
            else
            {
                m_connectorPcb = nullptr;
                CPL_SYSTEM_TRACE_MSG( SECT_, ("AsyncConnector: Failed tcp_connect(): %d", err) );
            }
        }
    }

    PICO_UNLOCK();
    return result;
}

err_t AsyncConnector::lwIPCb_connected( void* arg, struct tcp_pcb* newpcb, err_t err )
{
    // Note: Per the LWIP documentation: 'err' arg is unused, i.e. always ERR_OK

    AsyncConnector* theOne = (AsyncConnector*) arg;
    err_t           result = ERR_OK;
    PICO_CB_LOCK();

    // Trap errors
    if ( theOne == nullptr || newpcb == nullptr || theOne->m_clientPtr == nullptr )
    {
        result = ERR_VAL;
    }

    // Reject the request if there is already an active connection
    else if ( theOne->m_connectorPcb != nullptr )
    {
        tcp_abort( newpcb );
        result = ERR_ABRT;
    }

    else
    {
        // Set up the new 'file descriptor' for the connection
        theOne->m_connectionFd.lwipPcb = newpcb;
        Cpl::Io::Descriptor newfd      = (void*) &(theOne->m_connectionFd);

        // Set keep alive for the connection
        ip_set_option( newpcb, SOF_KEEPALIVE );

        // Inform the client of new connection
        if ( theOne->m_clientPtr->newConnection( newfd ) )
        {
            // Clear the client pointer now the connection succeeded/was-accepted.
            // The pointer is only needed/used to notify the client when the connection 
            // was refused (which is done via the 'tcp_err' callback)
            theOne->m_connectionFd.connnectorClientPtr = nullptr;
        }

        // Client rejected the connection
        else
        {
            // Housekeeping
            memset( &(theOne->m_connectionFd), 0, sizeof( Socket_T ) );

            // Cancel the connection
            tcp_abort( newpcb );
            result = ERR_ABRT;
        }
    }

    PICO_CB_UNLOCK();
    return result;
}


void AsyncConnector::poll() noexcept
{
    PICO_LOCK();
    cyw43_arch_poll();  // Note: this is only needed/meaningful when compiled with: PICO_CYW43_ARCH_POLL=1 
    PICO_UNLOCK();
}

void AsyncConnector::terminate() noexcept
{
    PICO_LOCK();

    // Close the stream. Note: 'connnectorClientPtr' will be NULL after the client accepts the connection
    if ( m_clientPtr && m_connectionFd.connnectorClientPtr == nullptr )
    {
        m_clientPtr->close();   // Note: This call closes 'm_connectorPcb' (because m_connectionFd.lwipPcb == m_connectorPcb)
    }

    // Close connector PCB (when there the connection is/was still-in-progress)
    else if ( m_connectionFd.connnectorClientPtr != nullptr && m_connectorPcb != nullptr )
    {
        tcp_close( m_connectorPcb );
    }


    // Clear my 'started state'
    m_clientPtr              = nullptr;
    m_connectorPcb           = nullptr;
    memset( &m_connectionFd, 0, sizeof( Socket_T ) );
    PICO_UNLOCK();
}
