# Colony.Pico

The Colony.Pico package is collection of my Raspberry Pico projects that are built using the Raspberry Pi Pico C/C++ SDK and the Colony.Core C++ Class library.

See the [Getting Started] section for setup and build instructions.

See the [Threads Example] section for details of the example application demonstrating the package's Threading model.

Doxygen documentation is located in the `docs/` directory 

Source code under the `src/Cpl` directory is licensed with a BSD licensing agreement (see the top/ directory).

Individual Pico packages have their own licenses (see the individual directories).


For more details about the `Colony.*` eco-system see 
  * https://github.com/johnttaylor/colony.core
  * https://github.com/johnttaylor/nqbp
  * https://github.com/johnttaylor/pim
  * http://integerfox.com/
  * And for a peek into the concepts behind the Colony stuff, there is a book [Patterns in the Machine:A Software Engineering Guide to Embedded Development](https://www.amazon.com/Patterns-Machine-Software-Engineering-Development-dp-1484264398/dp/1484264398/ref=mt_other?_encoding=UTF8&me=&qid=) availabe.

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
4. Either modify the `env.sh` script in the repo's root directory or create a new script that sets the compiler toolchain environment for the ARM cross compiler.  The current script only supports setup the compiler toolchain for the native GCC compiler.  _**Sorry** about the missing Linux support for this script, but I use a Windoze box for my open source work._
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
The Colony.Pico repository directory structure separates the source code directories from the directories where the builds are performed. In the following top-level directory structure, application builds and unit tests are built in the `projects/` and `tests/` directory trees,respectively. The source code resides under the `src/` or `xsrc/` directory trees.

    ```
    <workspace root>
    ├───docs
    ├───projects ; Build applications/released images
    ├───resources
    ├───scripts
    ├───src
    ├───tests ; Build unit tests
    ├───top
    └───xsrc
    ```

# Threads Example


