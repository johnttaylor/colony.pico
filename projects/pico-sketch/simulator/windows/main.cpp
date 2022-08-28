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

#include "../../app.h"
#include "../simulator.h"
#include "Cpl/Io/Socket/Win32/Connector.h"
#include "Cpl/Io/Socket/InputOutput.h"
#include "Cpl/System/Api.h"
#include "Cpl/Io/InputOutput.h"
#include "Cpl/Io/Stdio/StdIn.h"
#include "Cpl/Io/Stdio/StdOut.h"
#include "docopt-cpp/docopt.h"
#include "docopt-cpp/docopt_value.h"

static const char USAGE[] =
R"(Pico-sketch Simulation.

    Usage:
      pico-sketch [options]

    Options:
      -H HOST       Hostname for the Display Simulation. [Default: 127.0.0.1]
      -p PORT       The Display Simulation's Port number [Default: 5010]

      -v            Be verbose
      -h,--help     Show this screen.

)";

static std::map<std::string, docopt::value> args_;

/// Allocate memory for the console stream pointers
Cpl::Io::Stdio::StdIn  consoleInfd_;
Cpl::Io::Stdio::StdOut consoleOutfd_;
Cpl::Io::Input*        g_consoleInputFdPtr  = &consoleInfd_;
Cpl::Io::Output*       g_consoleOutputFdPtr = &consoleOutfd_;

////////////////////////////////////////////////////
int main( int argc, char* argv[] )
{
    // Parse the command line arguments
    args_ = docopt::docopt( USAGE, { argv + 1, argv + argc }, true );

    // Initialize Colony C++ library
    Cpl::System::Api::initialize();

    // Platform specific sockets: Connect to the Display simulation
    Cpl::Io::Descriptor                 socketFd;
    Cpl::Io::Socket::Win32::Connector   connector;
    if ( connector.establish( args_["-H"].asString().c_str(), args_["-p"].asLong(), socketFd ) != Cpl::Io::Socket::Connector::eSUCCESS )
    {
        printf( "\nERROR. Failed to connected to the Display Simulation. (host=%s, port=%d).\n",
                args_["-H"].asString().c_str(), 
                args_["-p"].asLong() );
        return 1;
    }
    Cpl::Io::Socket::InputOutput socketStream( socketFd );

    // Initializes the simulator
    platform_init( socketStream, socketStream );

    // Launch the application (on the simulation platform this method returns -->but don't the executable end)
    runApplication();
    for ( ;;)
    {
        Cpl::System::Api::sleep( 1000 );
    }

    // If I get here something BAD happen!
    return 1;
}
