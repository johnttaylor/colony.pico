# GUI for the Pico-Sketch Simulator

This Visual Studio C# Solution is used to build the Windows executable that 
provides the simulated Display, RGB LED, and buttons for the Pico-Sketch application.

The executable communicates with the Pico-Sketch application via TCP sockets.
The executable is a _listener_ on port `5010`.  The GUI application must be
launch prior to running the simulated Pico-Sketch console application.