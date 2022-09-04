# The Main Pattern: A Case Study
In my book - [Patterns in the Machine: A Software Engineering Guide to Embedded Development](https://www.amazon.com/Patterns-Machine-Software-Engineering-Development-dp-1484264398/dp/1484264398/ref=mt_other?_encoding=UTF8&me=&qid=) - I discuss the `Main Pattern`.  The *Main Pattern* states that an application is built by wiring together independent components and modules. The Main pattern consists of:

  - The resolution of interface references with concrete implementations.
  - The initialization and shutdown sequencing.
  - The optional sequencing (or runtime execution) of a set of components and modules. The “set of components and modules” can be the entire application, a sub-system, a feature, or any combination thereof.

The book also provides an [example Thermostat application](https://github.com/johnttaylor/pim) that was built using various best practices from the book including the *Main Pattern*.  To demonstrate and to some extent validate how effective the *Main Pattern* is I ported the application to a Raspberry Pi Pico board with a small graphic LCD.  The Pico projects are located `projects/Storm/Thermostat/Pico/`.  The rest of this README page describes the challenges, process, and the final outcome of the exercise. 

#### TL;DR
The *Patterns in the Machine* book takes the [Open/Close](https://en.wikipedia.org/wiki/Open%E2%80%93closed_principle) principle quit literally in that:
  > Adding new functionality should not be done by editing existing source code.
   
 In a perfect world, the Pico port should have be able to be done without changing of the original source code.  In that respect, the port and original application design *failed.*  The port required me to modify 6 existing files:
  - Two build script files (to move a common source directory to be platform specific).
  - Two Algorithm source files so that the Algorithm does not require a dedicate thread.
  - Two top level *Main Pattern* files to break out some non-platform specific initialization so as not to replicated start-up logic.

So not a an **A+** - but I claim it is definitely a pass grade :). If this was a real world scenario of having to port my product to new hardware and I only had to make the type of changes listed above - I would be hero.  I will go further and claim that you can build production embedded software that is change tolerant and extensible in the same amount of time (or less) than a monolithic approach that is prevalent today in the embedded space.


# Requirements
I gave myself the following requirements for porting the Thermostat example code to the Pico.
  - Execute the example code on the Pico hardware platform.
  - Add a partial, basic GUI to the application.
  - Include a functional simulator of the application with its GUI that executes on a PC
     
# Constraints
The example Thermostat application is a incomplete application in the senses that it has no actual temperature inputs, relay outputs, or User Interface.  It has interfaces for all of the above - I just didn't have the time when writing the book to flush out a complete product.  That said the application does contain the following:
  - A PID based Temperature Control algorithm that supports Heat/Cool systems with zero to two stage air conditioners and up to 3 stages of indoor heat.
  - Since there are no physical inputs/outputs, a *house simulator* thats provide a crude closed loop simulation of a house and the effects of Outdoor temperature, Indoor temperature, Setpoints, and the output capacity of the HVAC system. 
  - A Debug Console that provides commands to fully exercise and monitor the thermostat's control algorithm.
  - Persistent storage for the HVAC Installer and Home owner settings.
  - Interfaces (via the Data Model) for integrating temperature inputs and relay outputs.
  - Interfaces (once a gain via the Data Model) for constructing a UI.


## Pico platfrom
My Pico hardware consists of a Raspberry Pi [Pico board](https://www.raspberrypi.com/products/raspberry-pi-pico/) (aka a RP2040 dual core MCU) with a Pimoroni [Pico-Display](https://shop.pimoroni.com/products/pico-display-pack?variant=32368664215635) (1.14" graphic LCD with 4 momentary push button and a RGB LED).  

Because the Thermostat application uses/is-dependent on the [Colony.Core CPL C++ Class library](https://github.com/johnttaylor/colony.core/blob/master/README.md) - the Pico port will also use the same class library.  However, the C++ class library's currrent [threading support for Pico](https://github.com/johnttaylor/colony.pico/blob/main/README.md) only provides up to 2 thread - one for each core.

### Deltas
The following are features, sub-systems, architecture, etc. that do not port *well* (i.e. without some kind of change or extension) to the Pico.
  - The number of threads.  The Thermostat application running on its original target hardware consist of the 4 Threads listed below.  The Pico only has two threads.
    - Control Algorithm
    - Persistent Storage record server
    - Debug Shell
    - House Simulator
  - Due to the thread limitations on the Pico - the Pico port requires the use of using Periodic Scheduler runnable objects (vs event based Mailboxes).  This has an impact on how in thread initialization and shutdown is done.  I will explain later in more detail about difference betweens a Periodic Scheduler and a Mailbox.
  - Persistent Storage.  The original target hardware has external data flash for persist storage.  My Pico hardware (currently) does not have any application data persistent storage.
   


