# Colony.Pico

The Colony.Pico package is collection of my Raspberry Pico projects that are built using the Raspberry Pi Pico C/C++ SDK and the Colony.Core C++ Class library.
> The Colony.Core C++ library Package was developed specifically for embedded software development. And as such, was designed for:
> * Deterministic memory management (i.e. no heap usage after startup)
> * The constrained (memory and CPU cycles) environment of embedded systems. 

### General Info
- See the [Getting Started](#Getting-Started) section for setup and build instructions.

- See the [Threading Example](#Threading-Example) and [Threading Details](#Threading-Details) sections for details of the example application demonstrating the package's Threading model.

- Stream based IO when using the [PicoW](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html) native WIFI libraies.  See [TCP Streams](https://github.com/johnttaylor/colony.pico/blob/main/src/Cpl/Io/Tcp/README.md) for more details.

- There is a [Pico-Sketch application](https://github.com/johnttaylor/colony.pico/blob/main/projects/pico-sketch/README.md#GettingStarted) that illustrates who to build a functional simulator.

- A case study of the [*Main Pattern*](https://github.com/johnttaylor/colony.pico/blob/main/projects/Storm/Thermostat/Pico/README.md) by porting the PIM example [Thermostat application](https://github.com/johnttaylor/pim) to the Pico.

- There is a [Visual Studio](https://visualstudio.microsoft.com/vs/community/) Solution that provides *Intellisense* editing and navigation of the code base as well as GUI front end for GIT. Click [here](https://github.com/johnttaylor/colony.pico/blob/main/README-VisualStudio.md) for details 

- Doxygen documentation is located in the `docs/` directory. 

- Licensing:
  - Source code under the `src/` directory is licensed with a BSD licensing agreement (see the `top/` directory).
  - Individual Pico-SDK packages have their own licenses (see the individual directories).


- For more details about the `Colony.*` eco-system see 
  * https://github.com/johnttaylor/colony.core
  * https://github.com/johnttaylor/nqbp
  * https://github.com/johnttaylor/pim
  * https://github.com/johnttaylor/Outcast
  * http://integerfox.com/
  * And for a dive into the concepts behind the Colony stuff, there is a book: [Patterns in the Machine: A Software Engineering Guide to Embedded Development](https://www.amazon.com/Patterns-Machine-Software-Engineering-Development-dp-1484264398/dp/1484264398/ref=mt_other?_encoding=UTF8&me=&qid=) 
    

# Features
The Colony.Core C++ class library with extensions for the Raspberry RP200 MCU provides the following features. _Note: This is not an exhaustive list of the features_.
* Provides a Operating System Abstraction Layer (OSAL) that decouples the application from the target hardware/operating-system.
* Thread support for each MCU Core (no RTOS required)
  * Mutexes
  * Semaphores
  * Thread Local Storage
  * A maximum of two threads (i.e. one thread per core)
* Simultaneous Event and Periodic scheduling for threads/cores
  * Event flags (up to 32 flags per thread)
  * Software timers
  * Message based Inter-Thread-Communications (ITC)
  * Data Model changed notifications 
  * Cooperative monotonic periodic scheduling
* Data Model framework
  * The Data Model software architecture pattern is a data-oriented pattern where
    modules interact with each other via data instances (a.k.a. model points) with
    no direct dependencies between modules.
* Streams
  * Platform independent interfaces for operations that would typically be done 
    using Posix file descriptors or Windows file HANDLEs.
* A text-based command shell framework that can be used to interact with an application at runtime via an IO stream (e.g. UART, stdio, etc.)
* Text handling methods and String classes that do not use dynamic memory allocation.


   
# Getting Started
### Requirements:
* [Python](https://www.python.org/downloads/) 3.6 or higher (for the build scripts).
* The majority of the code is target independent, i.e. it can be compiled for many different targets (e.g. PI RP2040, Windows, Linux, etc.). For each target that you _plan to use_ a compiler tool chain needs to be installed.
  * For the Raspberry PI RP2040 the [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) cross compiler (`arm-none-eabi` variant) is required. 
* Git client installed. This is optional if, instead of cloning the repository, you want to work with a downloaded zip file from GitHub.
* [Raspberry PI Pico board](https://www.raspberrypi.com/products/raspberry-pi-pico/).  Other RP2040 based board should work, but you might have to override the default UART and LED pin assignments. _Caveat: I have only tested with the Pico board._

  __Notes:__
  * The package does not use's the Pico C/C++ SDK's CMake builds tools.
  * The package includes (a subset of) the [`pico-sdk`](https://github.com/raspberrypi/pico-sdk) so only the `colony.pico` needs to be cloned.
  * All builds are done via the command line, no IDE required.

### Setup
#### Linux
1. __Optionally__ install the GCC compiler on your Linux PC. Install the entire GCC toolchain and tools. After installation, it is assumed that the GCC compiler is in the command path.  This step is only need if you plan on building projects or unit tests that executes on a Linux host machine.
2. Install the [Arm-none-eabi GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads). The Raspberry PI Pico [Getting Started Guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) has detailed instructions for installing the toolchain.
3. Clone the `colony.pico` repository to your PC. The repo can be installed
anywhere on your PC; however, the root path to the repo location
__cannot__ contain any spaces.
4. Either modify the `env.sh` script in the repo's root directory or create a new script that sets the compiler toolchain environment for the ARM cross compiler.  The current script only supports setting up the compiler toolchain for the native GCC compiler.  _**Sorry** about the missing Linux support for this script, but I use a Windoze box for my open source work so I don't spend a lot time streamline developing under Linux._
5. Run the `env.sh` script using the source shell command to set environment for your targeted toolchain. The `env.sh` script only needs to be run  once per command window per compiler. Here is an example for setting the toolchain to build native Linux executables:
    ```
    ~/work/colony.pico $ source env.sh
    ```
 
#### Windows
1. __Optionally__ install a native compiler on your Windows PC. The package supports using the [Microsoft Visual Studio compiler](https://visualstudio.microsoft.com/downloads/) and the [GCC Mingw compiler](http://mingw-w64.org/doku.php). This step is only need if you plan on building projects or unit tests that executes on a Windows host machine.
2. Install the [Arm-none-eabi GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads). The Raspberry PI Pico [Getting Started Guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) has detailed instructions for installing the toolchain
3. Clone the `colony.pico` repository to your PC. The repo can be installed
anywhere on your PC; however, the root path to the repo location
__cannot__ contain any spaces. 
    
    __Caveat:__ Because Windows has limitation on the size of single command line string it is strongly recommended that the directory path (not including the repository directory name) to the root of the repository be short, i.e. less than 20 characters .  The command line length is an issue because the SDK requires __a lot__ of compiler/linker options - that include path details - to be specified at build time.
4. Set up supporting scripts so that, when building on Windows, the
build scripts can find your installed compilers. You must edit
these scripts for each installed compiler. These scripts are located in
the `top/compilers/` directory. There needs to be a Windows batch
file for each installed compiler. The individual scripts are responsible
for updating the environment for that compiler, for example, adding
the compiler to the Windows command path, setting environment
variables needed by the compiler or debug tools, and so on.
Also, each script provides a “friendly name” that is used to identify the
compiler when running the env.bat script. The actual batch file script names
are not important; that is, it does not matter what the file names are as long as you have a unique file name for each script. Here is an example of the `top/compilers/01-vcvars32-vc17.bat` file:
    ```
    01|@echo off
    02|IF "/%1"=="/name" ECHO:Visual Studio VC17 (32bit) compiler for Windows 
    03|
    04|call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
    ```
    Line 2 is where the “friendly name” is set. And line 4 simply calls the batch file supplied by Visual Studio. You will need to edit this line to match your local installation. Here is an example of the `top/compilers/04-gcc-arm-pico-v11.2`.bat batch file:
    ```
    01|echo off
    02|IF "/%1"=="/name" ECHO:GCC-ARM compiler for Raspberry Pi Pico & exit /b 0
    03|
    04|call set PATH=C:\compilers\gcc-arm-none-eabi\11.2_2022.02\bin;PATH%
    05|echo:GCC Arm-none-eabi Environment set (v11.2)
    ```
    Line 2 is where the “friendly name” is set. Line 4 defines where you installed the Arm cross compiler. You will need to edit this line to match your local installation.
5. Run the `env.bat` file. From a command window (i.e., a DOS box), change to the root of the cloned repository and from there run the `env.bat` script. It might look something like this:
    ```
    c:\work\colony.pico>env.bat
    Current toolchain: Visual Studio VC17 (32bit) compiler for Windows
    
    1 - Visual Studio VC17 (32bit) compiler for Windows
    2 - Visual Studio VC17 (64bit) compiler for Windows
    3 - MINGW64 v9.2.0 compiler for Windows (32bit and 64 bit)
    4 - GCC-ARM compiler for Raspberry Pi Pico
    ```

    To select a compiler toolchain, run the script again with the number of the compiler toolchain you want to use, for example:
    ```
    c:\work\colony.pico> env.bat 4
    GCC Arm-none-eabi Environment set (v11.2)
    ```
# Building
The Colony.Pico repository directory structure separates the source code directories from the directories where the builds are performed. In the following top-level directory structure, application builds and unit tests are built in the `projects/` and `tests/` directory trees, respectively. The source code resides under the `src/` or `xsrc/` directory trees.

    <workspace root>
        +-- docs            ; Doxygen documentation
        +-- projects        ; Build applications/released images
        +-- pkg.info        ; Support for Outcast
        +-- pkg.overlaid    ; Support for Outcast
        +-- src             ; Source code & test code for the Package
        +-- tests           ; Build unit tests
        +-- top             ; Miscellaneous documentation, compiler scripts, etc.
        \-- xsrc            ; External repositories.

   To build an application or unit test, simply navigate to a leaf directory inside the `projects/` or `tests/` directories and run the `nqbp.py` script. All projects and unit tests are built by running the `nqbp.py` script.

### Example (Windows host)
The following example terminal session that builds the example "threads" project. The example starts with running the `env.bat` script. However, the `env.bat` script only needs to be run once per compiler per terminal window.

    c:\work\colony.pico>env.bat

    NO TOOLCHAIN SET
    1 - Visual Studio VC17 (32bit) compiler for Windows
    2 - Visual Studio VC17 (64bit) compiler for Windows
    3 - MINGW64 v9.2.0 compiler for Windows (32bit and 64 bit)
    4 - GCC-ARM compiler for Raspberry Pi Pico

    c:\work\colony.pico>env.bat 4
    GCC Arm-none-eabi Environment set (v11.2)

    c:\work\colony.pico>cd projects\examples\cpl_2threads\uart\pico\windows\gcc
 
    c:\work\colony.pico\projects\examples\cpl_2threads\uart\pico\windows\gcc>nqbp.py
    ================================================================================
    = START of build for:  bob-uart
    = Project Directory:   C:\work\colony.pico\projects\examples\cpl_2threads\uart\pico\windows\gcc
    = Toolchain:           GCC Arm-Cortex (none-eabi) Compiler
    = Build Configuration: pico
    = Begin (UTC):         Sun, 14 Aug 2022 18:07:57
    = Build Time:          1660500477 (62f939fd)
    ================================================================================
    = Cleaning Built artifacts...
    = Cleaning Project and local Package derived objects...
    = Cleaning External Package derived objects...
    = Cleaning Absolute Path derived objects...
    =====================
    = Building Directory: src\Bsp\RP2040\Pico\gcc
    = Compiling: Api.cpp
    =
    = Archiving: library.a
    =====================
    = Building Directory: src\Cpl\Io\Serial\RP2040\Uart
    = Compiling: InputOutput.cpp
    =
    = Archiving: library.a
    =====================
    = Building Directory: xsrc\pico-sdk\src\rp2_common\pico_stdio_uart
    = Compiling: stdio_uart.c

    ...

    =====================
    = Building Project Directory:
    = Compiling: application.cpp
    = Compiling: Bob.cpp
    = Compiling: main.cpp
    = Creating Boot2 (aka the second stage bootloader) ...
    =====================
    = Linking...
    = Creating BIN file ...
    = Running Print Size...
       text    data     bss     dec     hex filename
     155612      28    7572  163212   27d8c bob-uart.elf
    ================================================================================
    = END of build for:    bob-uart
    = Project Directory:   C:\work\colony.pico\projects\examples\cpl_2threads\uart\pico\windows\gcc
    = Toolchain:           GCC Arm-Cortex (none-eabi) Compiler
    = Build Configuration: pico
    = Elapsed Time (hh mm:ss): 00 00:25
    ================================================================================
    c:\work\colony.pico\projects\examples\cpl_2threads\uart\pico\windows\gcc>

# Threading Example

### Overview
The example project located under `projects/example/cpl_2threads/` directory is a simple blinky-LED application with a command shell that uses two threads.

The application blinks the Pico board's LED at a runtime settable frequency and optionally outputs a status messages every time the LED state is toggled.  The LED application code executes on core0. The application also contains the Colony.Core's 'TShell' command processor (think Debug console) that runs over the UART0 or stdio-over-USB provided by the SDK. The TShell command processor executes on core1.


The LED flash frequency and the terse/verbose state of the application is set by the developer entering TShell commands to set/change the LED timing and the terse/verbose state.

Below is example output of the application (using UART0 for command processor). The **`dm write ...`** commands are entered by the developer.
```
**** APPLICATION START-UP *****
Cpl and the Bsp have been initialized.
Launching unit tests...
>> 00 00:00:00.000 (app) [core0] Hello.  I am the Bob example application.
>> 00 00:00:00.000 (app) [APP-BOB] Enabling scheduling

--- Your friendly neighborhood TShell. ---


$ >> 00 00:00:00.001 (app) [APP-BOB] Bob:delayModelPointChange()
>> 00 00:00:00.004 (app) [APP-BOB] Bob:verboseModelPointChange()
dm write {name:"delayTime",val:1000}
dm write {name:"verbose", val:true}
$ >> 00 00:00:26.087 (app) [APP-BOB] Bob:delayModelPointChange()
$ >> 00 00:00:26.090 (app) [APP-BOB] Bob:verboseModelPointChange()
>> 00 00:00:27.000 (app) [APP-BOB] Bob: edge counter=106
>> 00 00:00:28.000 (app) [APP-BOB] Bob: edge counter=107
>> 00 00:00:29.000 (app) [APP-BOB] Bob: edge counter=108
>> 00 00:00:30.000 (app) [APP-BOB] Bob: edge counter=109
dm write {name:"verbose", val:false}
$ >> 00 00:00:30.708 (app) [APP-BOB] Bob:verboseModelPointChange()
dm write {name:"delayTime",val:500}
dm write {name:"verbose", val:true}
$ >> 00 00:00:36.754 (app) [APP-BOB] Bob:delayModelPointChange()
$ >> 00 00:00:36.757 (app) [APP-BOB] Bob:verboseModelPointChange()
>> 00 00:00:36.800 (app) [APP-BOB] Bob: edge counter=116
>> 00 00:00:37.300 (app) [APP-BOB] Bob: edge counter=117
>> 00 00:00:37.800 (app) [APP-BOB] Bob: edge counter=118
>> 00 00:00:38.300 (app) [APP-BOB] Bob: edge counter=119
>> 00 00:00:38.800 (app) [APP-BOB] Bob: edge counter=120
>> 00 00:00:39.300 (app) [APP-BOB] Bob: edge counter=121
>> 00 00:00:39.800 (app) [APP-BOB] Bob: edge counter=122
>> 00 00:00:40.300 (app) [APP-BOB] Bob: edge counter=123
>> 00 00:00:40.800 (app) [APP-BOB] Bob: edge counter=124
dm write {name:"verbose", val:false}
$ >> 00 00:00:41.018 (app) [APP-BOB] Bob:verboseModelPointChange()

```


### Code Snippets
The example application was fairly simple and quick to create **once** the Colony.Core Cpl C++ class library was updated to support the RP2040 MCU.  Here is code for `main()` and the `runApplication()` method it calls.  The `core0MBox_` and `core1Mbox_` objects are  `Cpl::System::Runnable` instances (think the main/forever loop for each thread).

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
        
        // Start the Console/Trace output: Accepting the default UART Config parameters, e.g.   115200, 8N1
        Cpl::System::RP2040::startConsole();
        Cpl::System::RP2040::getConsoleStream().write( "\n**** APPLICATION START-UP *****\n" );
        Cpl::System::RP2040::getConsoleStream().write( "Cpl and the Bsp have been initialized.  \n" );
    
        // Set the stream of the console
        g_consoleInputFdPtr  = &(Cpl::System::RP2040::getConsoleStream());
        g_consoleOutputFdPtr = &(Cpl::System::RP2040::getConsoleStream());
    
        // Start the application
        runApplication();         // This method should never return
    }

    void runApplication()
    {
        CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Hello.  I am the Bob example application.") );
    
        // Create mock application thread 
        Cpl::System::Thread::create( core0Mbox_, "APP-BOB" );
    
        // Create a 'Scheduler' thread for the TShell to run in
        Cpl::System::Thread::create( core1Mbox_, "TSHELL" );
    
        // Start scheduling
        CPL_SYSTEM_TRACE_MSG( MY_APP_TRACE_SECTION, ("Enabling scheduling") );
        Cpl::System::Api::enableScheduling();
    }


The files containing these function are:
* [main.cpp](https://github.com/johnttaylor/colony.pico/blob/main/projects/examples/cpl_2threads/uart/pico/main.cpp)
* [application.h](https://github.com/johnttaylor/colony.pico/blob/main/projects/examples/cpl_2threads/application.h)
* [application.cpp](https://github.com/johnttaylor/colony.pico/blob/main/projects/examples/cpl_2threads/application.cpp)
* [Bob.h](https://github.com/johnttaylor/colony.pico/blob/main/projects/examples/cpl_2threads/Bob.h)
* [Bob.cpp](https://github.com/johnttaylor/colony.pico/blob/main/projects/examples/cpl_2threads/Bob.cpp)

__Caveat:__ Simple and quick are relative terms. _For someone unfamiliar with the Colony Core C++ Class library there is nothing simple or quick about creating this example application_.  The moral of the story here is that while Colony Core has reasons-to-it-madness - it does have learning curve.  Did I mention that there is [book](https://www.amazon.com/Patterns-Machine-Software-Engineering-Development-dp-1484264398/dp/1484264398/ref=mt_other?_encoding=UTF8&me=&qid=) that will help with the concepts ;-).

# Threading Details
The Colony.Pico package provides a concrete implemenation of the 
[Cpl::System](https://github.com/johnttaylor/colony.pico/blob/main/src/Cpl/System) namespace interfaces (think OSAL) for the Raspberry RP2040 MCU's dual cores,
i.e. a maximum of 2 threads - one per core.

### Platform Dependent Behaviors:

* [Thread](https://github.com/johnttaylor/colony.pico/blob/main/src/Cpl/System/Thread.h) - The first thread created runs on core0.  A SINGLE additional thread can be created that runs on core1.
* [Mutex](https://github.com/johnttaylor/colony.pico/blob/main/src/Cpl/System/Mutex.h) - Fully functional.  Note: The memory for the internal Mutex type is allocated from the Heap.
* [Global Lock](https://github.com/johnttaylor/colony.pico/blob/main/src/Cpl/System/GlobalLock.h) - The GlobalLock maps the RP2040 SDK's critical section.  There are some subtle details here - but it effective disables IRQs on the calling core AND provides mutual exclusion with respect to the other core.
* [Semaphore](https://github.com/johnttaylor/colony.pico/blob/main/src/Cpl/System/Semaphore.h) - Fully functional.  Note: The memory for the internal Semaphore type is allocated from the Heap.
* [Tls](https://github.com/johnttaylor/colony.pico/blob/main/src/Cpl/System/Tls.h) - Fully functional.
* [ElapsedTime](https://github.com/johnttaylor/colony.pico/blob/main/src/Cpl/System/ElapsedTime.h) - Fully functional.  
* [EventLoop](https://github.com/johnttaylor/colony.pico/blob/main/src/Cpl/System/EventLoop.h)   - Fully functional.  This includes EventFlags and Software Timers.
* [Periodic Scheduling](https://github.com/johnttaylor/colony.pico/blob/main/src/Cpl/System/PeriodicScheduler.h) - Fully functional.

### Application Start-up sequence:
1. MCU Resets.

2. Core0 begins executing. Core1 is in the sleep state.

3. The bootloader(s) execute and transfer execution to the Application.

4. The Application's C/C++ start-up code executes (e.g. crt0, low-level SDK initialization, etc. ).

5. The Application's C/C++ main() method is called.  
    - At this point the system is a bare metal system with interrupts disabled.
    - The application code should call `Cpl::System::Api::initialize()` ASAP.
    - The application should complete any required HW/BSP/App initialization 
      that needs to occur before 'thread scheduling' begins.
    - The Application needs to call `Cpl::System::Thread::Create()` to create 
      at least one 'thread'.  The application can create at most two 'threads'.
      The first thread created will execute on CORE0, the second thread executes
      on CORE1

6. The Application 'starts threading' by calling `Cpl::System::Thread::enableScheduling()`.

7. Once the 'threading' has been enable, the Application can optional create the second
   thread assuming only 1 thread was created prior to enableScheduling() call. 

__Notes:__ 
1. The second thread can be forcibly stopped (i.e. CORE1 put into the sleep state)
   by calling `Cpl::System::Thread::destroy()`.  If the second thread terminated itself,
   i.e. the Runnable object ran to completion, CORE1 will be put into the sleep state.
2. A second thread can be created 'again' after destory() has been called.
3. Calling destroy() on the first thread has NO effect.