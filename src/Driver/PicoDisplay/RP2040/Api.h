#ifndef Driver_PicoDisplay_RP2040_API_h
#define Driver_PicoDisplay_RP2040_API_h
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

    This file defines the interface for initializing the concrete implementation
    of the PicoDisplay driver using the actual hardware

 */

#include "colony_config.h"
#include "pimoroni_common.hpp"
#include "pico_display.hpp"


/// Pin for Button A
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_A_PIN
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_A_PIN      pimoroni::PicoDisplay::A
#endif

/// Pin for Button B
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_B_PIN
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_B_PIN      pimoroni::PicoDisplay::B
#endif

/// Pin for Button X
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_X_PIN
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_X_PIN      pimoroni::PicoDisplay::X
#endif

/// Pin for Button Y
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_Y_PIN
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_BUTTON_Y_PIN      pimoroni::PicoDisplay::Y
#endif

/// RGB LED: The pin for the RED PWM signal 
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_RED_PIN
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_RED_PIN       pimoroni::PicoDisplay::LED_R
#endif

/// RGB LED: The pin for the GREEN PWM signal 
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_GREEN_PIN  
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_GREEN_PIN     pimoroni::PicoDisplay::LED_G
#endif

/// RGB LED: The pin for the BLUE PWM signal 
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_BLUE_PIN  
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_RGB_BLUE_PIN      pimoroni::PicoDisplay::LED_B
#endif

/// The default screen orientation
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_LCD_ROTATION         
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_LCD_ROTATION      pimoroni::ROTATE_0
#endif

/// The default SPI bus
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_SPI_BUS
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_SPI_BUS           spi0
#endif

/// The default SPI pins
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_SPI_SCK
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_SPI_SCK           18
#endif

/// The default SPI pins
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_SPI_MOSI
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_SPI_MOSI          19
#endif

/// The default SPI pins
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_SPI_MISO 
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_SPI_MISO          16
#endif

/// The default SPI pins
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_SPI_CS
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_SPI_CS            17
#endif

/// The default SPI pins (Data/Command pin for the LCD controller)
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_SPI_DC
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_SPI_DC            16
#endif

/// The default PWM signal to the backlight
#ifndef OPTION_DRIVER_PICO_DISPLAY_RP2040_BACKLIGHT_PWM
#define OPTION_DRIVER_PICO_DISPLAY_RP2040_BACKLIGHT_PWM     20
#endif


///
namespace Driver {
///
namespace PicoDisplay {
///
namespace RP2040 {

/** This method is used to initialize the platform specific drivers
 */
void initialize();


} // End namespace(s)
}
}

#endif // end header latch
