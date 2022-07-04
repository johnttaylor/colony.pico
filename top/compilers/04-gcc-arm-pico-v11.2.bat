echo off
IF "/%1"=="/name" ECHO:GCC-ARM compiler for Raspberry Pi Pico & exit /b 0

call c:\compilers\gcc-arm-none-eabi-v11.2.bat
