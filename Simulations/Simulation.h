// 2021 -- 2022 Quentin Quadrat quentin.quadrat@gmail.com
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <https://unlicense.org>

#ifndef __USER_SIMULATION_H__
#  define __USER_SIMULATION_H__

// ****************************************************************************
//! \file Simulations are C++ code that is compiled as shared library and the
//! following functions will be loaded by the simulator application and run.
// ****************************************************************************

#  include "City/City.hpp"
#  include "Simulator/Simulator.hpp"

#ifdef __cplusplus
extern "C" {
#endif

//! \brief Function returning the simulation name.
const char* simulation_name();

//! \brief Function taking the Simulator as input and returns true when
//! the simulation shall halt.
bool halt_simulation_when(Simulator const& simulator);

//! \brief C fonction taking a City as input and allows to create the
//! desired city. This function returns the ego car.
Car& create_city(City& city);

// TODO faire un struct simulation { .simulation_name

#ifdef __cplusplus
}
#endif

#endif /* __USER_SIMULATION_H__ */
