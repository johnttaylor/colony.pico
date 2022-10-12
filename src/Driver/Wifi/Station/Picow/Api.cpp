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

#include "Driver/Wifi/Station/Connection.h"
#include "Cpl/System/Assert.h"
#include "pico/cyw43_arch.h"


using namespace Driver::Wifi::Station;

static bool                             started_;
static Connection::StateChangedFunc_T   cbfunc_;
static const char*                      ssid_;
static const char*                      password_;
static Connection::State_T              prevState_;
static Connection::Authentication_T     auth_;

//////////////////////////////////
bool Connection::start( const char*         ssid,
                        const char*         password,
                        Authentication_T    authenticationMethod,
                        StateChangedFunc_T  callbackFunc ) noexcept
{
    CPL_SYSTEM_ASSERT( ssid );
    CPL_SYSTEM_ASSERT( password );

    ssid_      = ssid;
    password_  = password;
    auth_      = authenticationMethod;
    cbfunc_    = callbackFunc;
    prevState_ = Connection::eLINK_DOWN;

    cyw43_arch_enable_sta_mode();
    if ( cyw43_arch_wifi_connect_async( ssid_, password_, auth_ ) == PICO_OK )
    {
        started_ = true;
        return true;
    }
    return false;
}

void Connection::poll() noexcept
{
    // Skip if not started
    if ( started_ )
    {
        int status = cyw43_tcpip_link_status( &cyw43_state, CYW43_ITF_STA );

        // Convert the cyw43 state to the interface state
        Connection::State_T currentState;
        switch ( status )
        {
        case CYW43_LINK_DOWN: currentState = Connection::eLINK_DOWN; break;
        case CYW43_LINK_JOIN: currentState = Connection::eLINK_JOINED; break;
        case CYW43_LINK_NOIP: currentState = Connection::eLINK_JOINED_NOIP; break;
        case CYW43_LINK_UP: currentState = Connection::eLINK_UP; break;
        case CYW43_LINK_FAIL: currentState = Connection::eLINK_FAILED; break;
        case CYW43_LINK_NONET: currentState = Connection::eLINK_NO_NETWORK; break;
        case CYW43_LINK_BADAUTH: currentState = Connection::eLINK_BAD_AUTHENTICATION; break;
        default: currentState = Connection::eLINK_FAILED; break;
        }

        // Notify the application when something changed
        if ( cbfunc_ && currentState != prevState_ )
        {
            (*cbfunc_)(currentState);
        }
        prevState_ = currentState;

        // Reconnect if the link has dropped
        if ( status < 0 )
        {
            cyw43_arch_wifi_connect_async( ssid_, password_, auth_ );
        }
    }
}


bool Connection::isConnected() noexcept
{
    return started_ && prevState_ == Connection::eLINK_UP;
}


void Connection::stop() noexcept
{
    if ( started_ )
    {
        cyw43_wifi_leave( &cyw43_state, CYW43_ITF_STA );
    }
}
