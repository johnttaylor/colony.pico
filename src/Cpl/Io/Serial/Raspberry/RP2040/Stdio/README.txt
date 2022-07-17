/** @namespace Cpl::Io::Serial::Raspberry::RP2040::Stdio

The Stdio namespace implements the Cpl::Io streams utilizing the Raspberry PI 
Pico C/C++ SDK's stdio interfaces.  The SDK's stdio interface supports using a 
physical UART on the MCU, or a virtual UART over USB (i.e. as a CDC USB device).
Which physical media is used is a compile/build time decision.

Note: The implement does NOT support the blocking semantics of the Cpl::Io
      streams.  The design decision was for the implementation to be compatible/
      useful on bare-metal systems, i.e. not threads required.

*/  


  