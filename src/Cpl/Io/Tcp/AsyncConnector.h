#ifndef Cpl_Io_Socket_Connector_h_
#define Cpl_Io_Socket_Connector_h_
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

#include "Cpl/Io/Descriptor.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Tcp {


/** This abstract class defines the interface for establishing/requesting
	a SIMPLE socket connection, i.e. make a "client connection".   When the
	connection is accepted by the remote host, the connector notifies the client. 
	
	The interface semantics are one Connector instance per connection.
 */
class AsyncConnector
{
	/** This class defines the callback mechanism used for accepting incoming
		TCP connections.
	 */
	class Client
	{
	public:
		/// Possible error codes when attempting to establish a connection
		enum Error_T {
			eERROR,         //!< Error occurred
			eREFUSED,       //!< Connection request was refused by the remote Host
		};

	public:
		/** This method is a callback method that is called when the remote host
			has accepted in connection request.

			Expected usage when the connection is accepted is to create (or
			activate) a Cpl::Io::Tcp::InputOutput instance with 'newFd'
		 */
		virtual void newConnection( Cpl::Io::Descriptor newFd ) noexcept = 0;

		/** This method is a callback method that is called when an error occurred
			when making the connection request.
		 */
		virtual void connectionFailed( Error_T errorCode ) noexcept = 0;
	};

public:
	/** Requests a client connection to the specified remote Host.  
	 */
	virtual void establish( Client&     client, 
							const char* remoteHostName, 
							int			portNumToConnectTo ) = 0;


public:
	/// Virtual destructor
	virtual ~AsyncConnector() {}
};


};      // end namespaces
};
};
#endif  // end header latch
