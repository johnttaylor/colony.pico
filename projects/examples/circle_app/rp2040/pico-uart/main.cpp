#include "../../app.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/RP2040/Console.h"
#include "Driver/Button/RP2040/Hal.h"
#include "Driver/LED/PimoroniPico/RedGreenBlue.h"



/// Allocate memory for the console stream pointers
Cpl::Io::Input*  g_consoleInputFdPtr;
Cpl::Io::Output* g_consoleOutputFdPtr;

#define APP_BUTTON_A_PIN_NUM    12
#define APP_BUTTON_B_PIN_NUM    13
#define APP_BUTTON_X_PIN_NUM    14
#define APP_BUTTON_Y_PIN_NUM    15

Driver::Button::PolledDebounced g_buttonA( { APP_BUTTON_A_PIN_NUM, true } );
Driver::Button::PolledDebounced g_buttonB( { APP_BUTTON_B_PIN_NUM, true } );
Driver::Button::PolledDebounced g_buttonX( { APP_BUTTON_X_PIN_NUM, true } );
Driver::Button::PolledDebounced g_buttonY( { APP_BUTTON_Y_PIN_NUM, true } );


#define APP_LED_RGB_R       6
#define APP_LED_RGB_G       7
#define APP_LED_RGB_B       8

static Driver::LED::PimoroniPico::RedGreenBlue rgbLEDDriver_( APP_LED_RGB_R , APP_LED_RGB_G, APP_LED_RGB_B );
Driver::LED::RedGreenBlue*                     g_rgbLEDDriverPtr = &rgbLEDDriver_;

/*-----------------------------------------------------------*/
int main( void )
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Initialize the board
    Bsp_Api_initialize();
    driverButtonHalRP2040_initialize( g_buttonA.getHandle() );
    driverButtonHalRP2040_initialize( g_buttonB.getHandle() );
    driverButtonHalRP2040_initialize( g_buttonX.getHandle() );
    driverButtonHalRP2040_initialize( g_buttonY.getHandle() );

    // Start the Console/Trace output: Accepting the default UART Config parameters, e.g. 115200, 8N1
    Cpl::System::RP2040::startConsole();
    Cpl::System::RP2040::getConsoleStream().write( "\n**** APPLICATION START-UP *****\n" );
    Cpl::System::RP2040::getConsoleStream().write( "Cpl and the Bsp have been initialized.\n" );

    // Set the stream of the console
    g_consoleInputFdPtr  = &(Cpl::System::RP2040::getConsoleStream());
    g_consoleOutputFdPtr = &(Cpl::System::RP2040::getConsoleStream());

    // Start the application
    runApplication();         // This method should never return
    return 0;
}
