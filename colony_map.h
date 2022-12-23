// This file exists ONLY to make Visual Studio's Intellisense happy. This 
// file SHOULD NEVER be a 'part' of your project(s).
// 


// Provide basic/common mappings.  
#include "Cpl/System/RP2040/mappings_.h"
#include "Cpl/Text/_mappings/_arm_gcc_rp2040/strapi.h"

// My/Default BSP
#include "Bsp/RP2040/Pico/gcc/Api.h"

// FOR SOME REASON - the PICO SDK header files confuses VS Intellisense - so we 
// brute force the definition of uint32_t.  Since THIS FILE is not part of the
// actual build - it shouldn't break anything.
typedef unsigned int       uint32_t;

#error this should NOT be included
