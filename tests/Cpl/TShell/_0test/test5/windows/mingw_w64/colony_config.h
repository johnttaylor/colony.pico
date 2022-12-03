#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

// Enable 'TestB'
#define BUILD_TEST_B_

// Enable security
#define USE_CPL_TSHELL_PROCESSOR_SILENT_WHEN_PUBLIC
#define OPTION_TSHELL_CMD_COMMAND_DEFAULT_PERMISSION_LEVEL Cpl::TShell::Security::eSYSTEM


// Enable Trace (but NOT on code coverage builds)
#ifndef BUILD_VARIANT_WIN32
#define USE_CPL_SYSTEM_TRACE
#endif

#endif
