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

#include "loopback.h"
#include "Cpl/System/Thread.h"
#include "Cpl/Itc/PeriodicScheduler.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/Io/Tcp/InputOutput.h"

#define SECT_   "_0test"

using namespace Cpl::Io::Tcp;

class ListenerClient : public AsyncListener::Client
{
public:
    bool m_connected;

public:
    ListenerClient()
        :m_connected( false )
    {
    }

public:
    bool newConnection( Cpl::Io::Descriptor newFd, const char* rawConnectionInfo ) noexcept
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Accepted incoming connection. Remote Host: %s", rawConnectionInfo) );
        activate( newFd );
        m_connected = true;
        return true;
    }

public:
    void consumeInput()
    {
        if ( m_connected )
        {
            int  bytesRead;
            char inBuf[128];
            if ( read( inBuf, sizeof( inBuf ), bytesRead ) )
            {
                if ( bytesRead > 0 )
                {
                    //CPL_SYSTEM_TRACE_MSG( SECT_, ("LIST: Bytes in: %d", bytesRead) );
                    CPL_SYSTEM_TRACE_MSG( SECT_, ("LIST: Bytes in: %d [%.*s]", bytesRead, bytesRead, inBuf) );
                    //int bytesWritten;
                    //if ( write( inBuf, bytesRead, bytesWritten ) )
                    //{
                    //    CPL_SYSTEM_TRACE_MSG( SECT_, ("LIST:   echoed: %d", bytesWritten) );
                    //}
                    //else
                    //{
                    //    CPL_SYSTEM_TRACE_MSG( SECT_, ("LIST: WRITE FAILED") );
                    //    m_connected = false;
                    //}
                }
            }
            else
            {
                CPL_SYSTEM_TRACE_MSG( SECT_, ("LIST: READ FAILED") );
                m_connected = false;
            }
        }
    }
};

#define TEST_STRING1 "Hi there.  My name is bob"
#define TEST_STRING2 "Nice to meet you bob!"


#define STATE_WAITING_INCOMING_DATA     0
#define STATE_DELAYING                  1

#ifndef OPTION_DELAY_INTERVAL_MS
#define OPTION_DELAY_INTERVAL_MS       1000
#endif

class ConnectorClient : public AsyncConnector::Client
{
public:
    bool                      m_connected;
    unsigned long             m_timeMark;
    int                       m_state;
    bool                      m_string1;

public:
    ConnectorClient()
        :m_connected( false )
    {
    }

public:
    bool newConnection( Cpl::Io::Descriptor newFd )  noexcept
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Connection ESTABLISHED") );
        activate( newFd );
        m_connected = true;

        m_state   = STATE_WAITING_INCOMING_DATA;
        m_string1 = true;
        sendData();
        return true;
    }

    const char* testString()
    {
        return m_string1 ? TEST_STRING1 : TEST_STRING2;
    }

    void sendData()
    {
        char outBuf[128];
        int  bytesWritten;
        strcpy( outBuf, testString() );
        if ( write( outBuf, strlen( outBuf ), bytesWritten ) )
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("CONN: Bytes OUT: %d", bytesWritten) );
        }
        else
        {
            CPL_SYSTEM_TRACE_MSG( SECT_, ("CONN: WRITE FAILED") );
            m_connected = false;
        }
    }

    void connectionFailed( Error_T errorCode ) noexcept
    {
        CPL_SYSTEM_TRACE_MSG( SECT_, ("Connection FAILED: error=%X", errorCode) );
    }

public:
    void consumeInput()
    {
        if ( m_connected )
        {
            if ( m_state == STATE_WAITING_INCOMING_DATA )
            {
                int  bytesRead;
                char inBuf[128];
                if ( read( inBuf, sizeof( inBuf ), bytesRead ) )
                {
                    if ( bytesRead > 0 )
                    {
                        CPL_SYSTEM_TRACE_MSG( SECT_, ("CONN: Bytes  IN: %d. %.*s", bytesRead, bytesRead, inBuf) );
                        if ( memcmp( inBuf, testString(), strlen( testString() ) ) != 0 )
                        {
                            CPL_SYSTEM_TRACE_MSG( SECT_, ("ERROR: Expected: [%s]", testString()) );
                            close();
                            m_connected = false;
                        }
                        else
                        {
                            m_state    = STATE_DELAYING;
                            m_string1  = !m_string1;
                            m_timeMark = Cpl::System::ElapsedTime::milliseconds();
                        }
                    }
                }
                else
                {
                    CPL_SYSTEM_TRACE_MSG( SECT_, ("CONN: READ FAILED") );
                    m_connected = false;
                }
            }

            if ( m_state == STATE_DELAYING )
            {
                if ( Cpl::System::ElapsedTime::expiredMilliseconds( m_timeMark, OPTION_DELAY_INTERVAL_MS ) )
                {
                    sendData();
                    m_state = STATE_WAITING_INCOMING_DATA;
                }
            }
        }
    }
};

//////////////////////////////////////////////
int                           portNum_;
Cpl::Io::Tcp::AsyncListener*  listenerPtr_;
ListenerClient                listenClient_;
Cpl::Io::Tcp::AsyncConnector* connectorPtr_;
ConnectorClient               connectorClient_;


static void tcpScan_( Cpl::System::ElapsedTime::Precision_T currentTick, bool atLeastOneIntervalExecuted )
{
    listenerPtr_->poll();
    connectorPtr_->poll();
    listenClient_.consumeInput();
    //connectorClient_.consumeInput();
}

static Cpl::System::PeriodicScheduler::Interval_T intervals_[] =
{
    CPL_SYSTEM_PERIODIC_SCHEDULAR_END_INTERVALS // Create an empty set of intervals - actual intervals not used for the test
};

static void beginThreadProcessing( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    listenerPtr_->startListening( listenClient_, portNum_ );
    //connectorPtr_->establish( connectorClient_, "127.0.0.1", portNum_ );
}
static void endThreadProcssing( Cpl::System::ElapsedTime::Precision_T currentTick )
{
    listenerPtr_->terminate();
    //connectorPtr_->terminate();
}


static Cpl::Itc::PeriodicScheduler scheduler_( intervals_,
                                               beginThreadProcessing, endThreadProcssing,
                                               nullptr,
                                               Cpl::System::ElapsedTime::precision,
                                               tcpScan_ );


////////////////////////////////////////////////////////////////////////////////

int runTest( Cpl::Io::Tcp::AsyncListener& listener, Cpl::Io::Tcp::AsyncConnector& connector, int portNum )
{
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Enter: TCP Loopback test") );

    // Cache the UUTs
    portNum_      = portNum;
    listenerPtr_  = &listener;
    connectorPtr_ = &connector;

    // Create thread for the test to run in
    Cpl::System::Thread::create( scheduler_, "TEST" );

    // Start scheduling
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Enabling scheduling") );
    Cpl::System::Api::enableScheduling();
    CPL_SYSTEM_TRACE_MSG( SECT_, ("Scheduling Enabled!") );

    // The test assumes that enableScheduling() never returns -->so make sure we don't return
    for ( ;;)
    {
        Cpl::System::Api::sleep( 1000 );
    }
}


