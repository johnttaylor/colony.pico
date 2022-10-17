#ifndef Cpl_Io_Tcp_lwIP_Pico2_Connector_h_
#define Cpl_Io_Tcp_lwIP_Pico2_Connector_h_
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

#include "Cpl/Io/Tcp/AsyncConnector.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Tcp {
///
namespace lwIP {
///
namespace Picow {


/** This class implements the Asynchronous Connector.
 */
class AsyncConnector: public Cpl::Io::Tcp::AsyncConnector

{
public:
    /// Constructor
    AsyncConnector(){}

    /// Destructor
    ~AsyncConnector(){}

public:
    /// See Cpl::Io::Tcp::AsyncConnector
    bool establish( Client&     client,
                    const char* remoteHostName,
                    int         portNumToConnectTo )
    {
        return false;
    }

    /// See Cpl::Io::Tcp::AsyncConnector
    void poll() noexcept{}

    /// See Cpl::Io::Tcp::AsyncConnector
    void terminate() noexcept{}
};


};      // end namespaces
};
};
};
};
#endif  // end header latch
