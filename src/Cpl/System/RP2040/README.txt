/** @namespace Cpl::System::RP2040

Concrete implementation of the platform specific classes/features of the 
Cpl::System namespace interfaces for the Raspberry RP2040 MCU's dual cores,
i.e. a maximum of 2 threads - one per core.

Platform Dependent Behaviors:

Thread      - The system also has one thread running on core0.  A SINGLE 
              additional thread can be created that runs on core1.

Mutex       - Fully functional.

GlobalLock  - The GlobalLock maps the RP2040 SDK's critical section.  There are
              some subtle details here - but it effective disables IRQs on 
              the calling core AND provides mutual exclusion with respect to
              the other core.

Semaphore   - Fully functional.  

Tls         - Fully functional.

ElapsedTime - Fully functional.  
              
EventLoop   - Fully functional.  This includes EventFlags and Software Timers.

*/  


  