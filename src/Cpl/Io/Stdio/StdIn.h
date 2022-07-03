#ifndef Cpl_Io_Stdio_StdIn_h_
#define Cpl_Io_Stdio_StdIn_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Io/Stdio/Input_.h"

///
namespace Cpl {
///
namespace Io {
///
namespace Stdio {


/** This concrete class represents the C-library 'stdin' stream.

    NOTE: Many instances of this class may be created - even
          though there is only ONE stdin.  This is OK because
          all instance reference the systems/platform 'stdin'
 */
class StdIn : public Input_
{
public:
    /// Constructor
    StdIn();


public:
    /** Overriding inherited behavior so as to NOT close the actual native
        stream. In general closing stdin/stdout/stderr is bad thing.
     */
    void close();
};

};      // end namespaces
};
};
#endif  // end header latch
