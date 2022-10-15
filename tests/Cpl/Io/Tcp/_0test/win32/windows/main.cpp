#include "Cpl/Io/Tcp/Win32/AsyncListener.h"
#include "Cpl/Io/Tcp/Win32/AsyncConnector.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/Io/Tcp/_0test/loopback.h"

int main( int argc, char* argv[] )
{
    // Initialize Colony
    Cpl::System::Api::initialize();

    // Platform specific listener
    Cpl::Io::Tcp::Win32::AsyncListener  listener;
    //Cpl::Io::Tcp::Win32::AsyncConnector connector;

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION("_0test");
    CPL_SYSTEM_TRACE_ENABLE_SECTION( "*Cpl::Io::Tcp" );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eVERBOSE );

    // Run the test(s)
    return runTest( listener,/* connection, */ OPTION_TCP_TEST_PORT_NUM );
}
