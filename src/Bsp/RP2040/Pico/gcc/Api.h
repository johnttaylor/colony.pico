#ifndef Bsp_RP2040_Pico_Api_h_
#define Bsp_RP2040_Pico_Api_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file


    This BSP is developed/build with a 'just-in-time' approach.  This means
    as functional is added to the BSP incrementally as there is 'client'
    need for.  This BSP does not claim to provide full/complete functional
    and/or APIs for everything the board supports.


    DO NOT include this file directly! Instead include the generic BSP
    interface - src/Bsp/Api.h - and then configure your project's
    'colony_map.h' to include THIS file.

*----------------------------------------------------------------------------*/


#include "colony_config.h"
#include "pico/sync.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"



//////////////////////////////////////////////////////////
/// Mappings for Generic APIs
//////////////////////////////////////////////////////////

/// Generic API
#define Bsp_Api_nop_MAP()                       __asm("nop")                

/// Generic API
#define Bsp_Api_disableIrqs_MAP                 Bsp_enterCriticalSection

/// Generic API
#define Bsp_Api_enableIrqs_MAP                  Bsp_exitCriticalSection

/// Generic API
#define Bsp_Api_pushAndDisableIrqs_MAP          Bsp_enterCriticalSection    // FIXME: This NOT the correct way to implement nested 'disables' on the PICO

/// Generic API
#define Bsp_Api_popIrqs_MAP()                   Bsp_exitCriticalSection     // FIXME: This NOT the correct way to implement nested 'disables' on the PICO




/// Default Mapping for the "debug LED 1"
#ifndef OPTION_BSP_DEBUG_LED1_INDEX
#define OPTION_BSP_DEBUG_LED1_INDEX             PICO_DEFAULT_LED_PIN
#endif

/// Default Mapping for the "debug LED 2"
#ifndef OPTION_BSP_DEBUG_LED2_INDEX
#define OPTION_BSP_DEBUG_LED2_INDEX             // The board ONLY has 1 LED
#endif



/// Generic API
#define Bsp_Api_turnOn_debug1_MAP()             gpio_put( OPTION_BSP_DEBUG_LED1_INDEX, true )

/// Generic API
#define Bsp_Api_turnOff_debug1_MAP()            gpio_put( OPTION_BSP_DEBUG_LED1_INDEX, false )

/// Generic API
#define Bsp_Api_toggle_debug1_MAP()             gpio_put( OPTION_BSP_DEBUG_LED1_INDEX, !gpio_get_out_level(OPTION_BSP_DEBUG_LED1_INDEX) )


/// Generic API
#define Bsp_Api_turnOn_debug2_MAP()             // Not supported. There is one LED on the PICO board

/// Generic API
#define Bsp_Api_turnOff_debug2_MAP()            // Not supported. There is one LED on the PICO board

/// Generic API
#define Bsp_Api_toggle_debug2_MAP()             // Not supported. There is one LED on the PICO board


//////////////////////////////////////////////////////////
/// Target/Board specific Functions 
//////////////////////////////////////////////////////////

/// Default UART Settings
#define BSP_DEFAULT_UART_HANDLE     uart0

/// Default UART Settings
#define BSP_DEFAULT_UART_TX_PIN     0

/// Default UART Settings
#define BSP_DEFAULT_UART_RX_PIN     1

/// Default RX FIFO IRQ threshold
#ifndef OPTION_BSP_DEFAULT_UART0_RX_FIFO_IRQ_THRESHOLD
#define OPTION_BSP_DEFAULT_UART0_RX_FIFO_IRQ_THRESHOLD      2   // 0=1/8 full, 1=1/4 full, 2=1/2 full, 3=3/4 full, 4=7/8 full
#endif

/// Default RX FIFO IRQ threshold
#ifndef OPTION_BSP_DEFAULT_UART1_RX_FIFO_IRQ_THRESHOLD
#define OPTION_BSP_DEFAULT_UART1_RX_FIFO_IRQ_THRESHOLD      2   // 0=1/8 full, 1=1/4 full, 2=1/2 full, 3=3/4 full, 4=7/8 full
#endif

/// Default TX FIFO IRQ threshold
#ifndef OPTION_BSP_DEFAULT_UART0_TX_FIFO_IRQ_THRESHOLD
#define OPTION_BSP_DEFAULT_UART0_TX_FIFO_IRQ_THRESHOLD      3   // 0=1/8 full, 1=1/4 full, 2=1/2 full, 3=3/4 full, 4=7/8 full
#endif

/// Default TX FIFO IRQ threshold
#ifndef OPTION_BSP_DEFAULT_UART1_TX_FIFO_IRQ_THRESHOLD
#define OPTION_BSP_DEFAULT_UART1_TX_FIFO_IRQ_THRESHOLD      3   // 0=1/8 full, 1=1/4 full, 2=1/2 full, 3=3/4 full, 4=7/8 full
#endif

/// Expose the 'global' critical section (so that mappings can be inlined)
extern critical_section_t g_bspGlobalCritSec_;

/** Enter a Critical section as defined by the SDK.  This method can ONLY
    be called after Bsp_Api_initialize() has been called.

    From the SDK: A critical section is non-reentrant, and provides mutual 
    exclusion using a spin-lock to prevent access from the other core, and 
    from (higher priority) interrupts on the same core. It does the former 
    using a spin lock and the latter by disabling interrupts on the calling 
    core.
 */
inline void Bsp_enterCriticalSection()
{
    critical_section_enter_blocking( &g_bspGlobalCritSec_ );
}

/** Exits a Critical section.  See description of Bsp_enterCriticalSection()
    for additional details.
 */
inline void Bsp_exitCriticalSection()
{
    critical_section_exit( &g_bspGlobalCritSec_ );
}

#endif  // end header latch
