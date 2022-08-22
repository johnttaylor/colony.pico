/** @namespace Driver::TPipe

The 'TPipe' namespace provides a point-to-point full duplex 'pipe' used 
to pass text based commands between to two end points.  Each command is 
encapsulated in a HDLC like frame (i.e. Start-of-Frame, End-of-Frame characters,
etc.).  The first token in the frame is a 'command verb'.  The command verb and
any additional frame data is application specific.

The connection between the two end-point is done by using two Cpl::Io streams,
one each for TX and RX.  The physical media is defined by the concrete stream
classes.  

A Pipe can have many 'transmit' and 'receive' clients.  

Transmitting frames is done using synchronous ITC messages to guarantee the 
transmission of each frame is done in the Pipe's TX thread, i.e. frames are
atomic with respect to other frames/TX-clients.

Incoming frames are processed by Clients registering frame handlers that 
identify the individual frames/command they process.  The frame handlers execute 
in the Pipe's RX thread.  Each frame handler is responsible for providing 
thread-safe with respect to the application.

An example usage of the TPipe to provide a TCP Socket connection between
a simulated firmware application where the socket connection is to external
program that provides a simulated display and simulated button inputs (to the
firmware simulation)

*/  


  