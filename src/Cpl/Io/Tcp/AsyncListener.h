#ifndef Cpl_Io_Tcp_AsyncListener_h_
#define Cpl_Io_Tcp_AsyncListener_h_
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

#include "Cpl/Io/Tcp/InputOutput.h"


///
namespace Cpl {
///
namespace Io {
///
namespace Tcp {


/** This abstract class defines the interface for a SIMPLE socket listener. A
	socket listener 'listens' for potential TCP/IP socket connections.  When a
	request for a connection comes in, the listener notifies the client.  
	
	The listener accepts only one connection at a time.

	The interface is NOT thread safe. This includes the client callback function, 
	i.e. no guarantees on what thread the callback function executes in. However, 
	the constructed IO Streams are thread safe.
 */
class AsyncListener 

{
public:
	/** This class defines the callback mechanism used for accepting incoming
		TCP connections.
	 */
	class Client
	{
	public:
		/** This method is a callback method that is called when the listener
			has accepted in incoming socket request.  It is up the client to
			determine if the application will accept or reject the TCP 
			connection.  If the client rejects the connection, it needs to 
			return false, else returns true.

			Expected usage when the connection is accepted is to create (or 
			activate) a Cpl::Io::Tcp::InputOutput instance with 'newFd'
		 */
		virtual bool newConnection( Cpl::Io::Descriptor newFd, const char* rawConnectionInfo ) noexcept = 0;


	public:
		/// Virtual destructor
		virtual ~Client() {}
	};



public:
	/// Starts the Listener listening.  
	virtual void startListening( Client& client, 
								 int	 portNumToListenOn ) noexcept = 0;

	/** This method must be called periodically to service the listen/connection
		status
	*/
	virtual void poll() noexcept = 0;

	/// Shuts down the Listener.  
	virtual void terminate() noexcept = 0;


public:
	/// Virtual destructor
	virtual ~AsyncListener() {}
};


};      // end namespaces
};
};
#endif  // end header latch
