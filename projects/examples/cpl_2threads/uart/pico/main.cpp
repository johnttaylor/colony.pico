#include "../../application.h"
#include "Bsp/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/Api.h"
#include "Cpl/TShell/Cmd/Threads.h"
#include "Cpl/System/RP2040/Console.h"

// Allocate memory for the stream pointers
Cpl::Io::Input*  g_consoleInputFdPtr;
Cpl::Io::Output* g_consoleOutputFdPtr;

/*-----------------------------------------------------------*/
int main(void)
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Initialize the board
    Bsp_Api_initialize();

    CPL_SYSTEM_TRACE_ENABLE();
    CPL_SYSTEM_TRACE_ENABLE_SECTION( MY_APP_TRACE_SECTION );
    CPL_SYSTEM_TRACE_SET_INFO_LEVEL( Cpl::System::Trace::eINFO );

    // Start the Console/Trace output: Accepting the default UART Config parameters, e.g. TX/RX Pin assignments, 115200, 8N1, etc.
    Cpl::System::RP2040::Console::start();

    // Run the Application (the call never returns!)
    g_consoleInputFdPtr  = &Cpl::System::RP2040::Console::getStream();
    g_consoleOutputFdPtr = &Cpl::System::RP2040::Console::getStream();
    runApplication();
    return 0;
}
