#include "../../../../application.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/RP2040/Console.h"




/// Allocate memory for the console stream pointers
Cpl::Io::Input*  g_consoleInputFdPtr;
Cpl::Io::Output* g_consoleOutputFdPtr;


/*-----------------------------------------------------------*/
int main(void)
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Initialize the board
    Bsp_Api_initialize();

    // Enable Tracing
    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( MY_APP_TRACE_SECTION );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );
    
    // Start the Console/Trace output: Accepting the default UART Config parameters, e.g. 115200, 8N1
    Cpl::System::RP2040::Console::start();
    Cpl::System::RP2040::Console::getStream().write( "\n**** APPLICATION START-UP *****\n" );
    Cpl::System::RP2040::Console::getStream().write( "Cpl and the Bsp have been initialized.\n" );
    Cpl::System::RP2040::Console::getStream().write( "Launching unit tests...\n" );

    // Set the stream of the console
    g_consoleInputFdPtr  = &(Cpl::System::RP2040::Console::getStream());
    g_consoleOutputFdPtr = &(Cpl::System::RP2040::Console::getStream());

    // Start the application
    runApplication();         // This method should never return
}
