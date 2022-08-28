#ifndef simulator_h
#define simulator_h
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

    This file defines the top level interface(s) to the Application's 'simulator'

 */

#include "Cpl/Io/Input.h"
#include "Cpl/Io/Output.h"

/** This method is used to initialize the 'simulator'
 */
void platform_init( Cpl::Io::Input& tpipeInStream, Cpl::Io::Output& tpipeOutStream );

#endif // end header latch
