#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

// Common Stuff
#include "../../../../colony_config.h"

#define OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD 10  // 10ms -->Windoze really can't go any 'faster' than this

// Force DOS newline
#define USE_CPL_IO_NEW_LINE_STANDARD   CPL_IO_NEW_LINE_NATIVE

// Work around the Windows console
#define USE_CPL_IO_STDIO_WIN32_STDIN_CONSOLE_HACK

#define USE_PICO_SKETCH_RGB565
#endif
