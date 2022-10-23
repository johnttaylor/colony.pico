/** @namespace Cpl::Io::Tcp::lwIP::Picow

The 'Picow' namespace implements the TCP interfaces for the Picow platform. The
dependencies for the implement are:

- The Pico C/C++ SDK.  
- Built with the pico_cyw43_arch set to polling or thread_safe_background
    CYW43_LWIP=1
    PICO_CYW43_ARCH_THREADSAFE_BACKGROUND=1 OR PICO_CYW43_ARCH_POLL=1

- Buildt with the lwIP config:
    NO_SYS=1    


NOTE: The implementation of this interface should be consider experimental 
      until further notice (10-16-2022)
*/  


  