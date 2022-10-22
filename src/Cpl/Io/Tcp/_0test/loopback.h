#ifndef loopback_h_
#define loopback_h_
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
/** @file 

    NOTE: You can use the compile switch: USE_TCP_LOOPBACK_TEST_SERVER_ONLY
          to build the test application to be a TCP Listener only, i.e. you
          can 'telnet' to the Test application and it will echo back the
          clients input
 */

#include "colony_config.h"
#include "Cpl/Io/Tcp/AsyncListener.h"
#include "Cpl/Io/Tcp/AsyncConnector.h"

#ifndef OPTION_TCP_TEST_PORT_NUM
#define OPTION_TCP_TEST_PORT_NUM    5002
#endif

int runTest( Cpl::Io::Tcp::AsyncListener& listener, Cpl::Io::Tcp::AsyncConnector& connector, int portNum );


#endif  // end header latch
