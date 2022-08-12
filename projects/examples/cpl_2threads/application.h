#ifndef Application_h
#define Application_h
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2019  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file

	This file define the top interface to the application.

	The following Commands can be used to control Bob from the Debug Console:

		dm write {name:"delayTime",val:100}
		dm write {name:"delayTime",val:500}

		dm write {name:"verbose",val:true}
		dm write {name:"verbose",val:false}
 */

#include "Cpl/Io/Input.h"
#include "Cpl/Io/Output.h"

/// Trace section for the application
#define MY_APP_TRACE_SECTION		"app"


/// Let the platform specific code create the Console stream
extern Cpl::Io::Input* g_consoleInputFdPtr;

/// Let the platform specific code create the Console stream
extern Cpl::Io::Output* g_consoleOutputFdPtr;

/** This method is used to start the application.  This method is required
	to NEVER return.

	This method assumes that the CPL C++ library has been initialize and basic
	HW/Board initialization has been done.
 */
void runApplication();


#endif  // end header latch
