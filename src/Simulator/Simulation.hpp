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

#ifndef SIMULATION_HPP
#  define SIMULATION_HPP

#  include "Common/DynamicLoader.hpp"

class Simulator;
class City;
class Car;

// ****************************************************************************
//! \brief Base Simulation structure holding C functions to set and configurate
//! simulations.
// ****************************************************************************
struct Scenario
{
    //! \brief C fonction returning the name of the simulation.
    std::function<const char* (void)> name;
    //! \brief C fonction taking a City as input and allows to create the
    //! desired city (roads, parking, vehicles ...). This function returns the
    //! ego car.
    std::function<Car&(City&)> create;
    //! \brief C function taking the Simulator as input and returns true when
    //! the simulation shall halt (simulation duration, collision, reach a given
    //! position ...).
    std::function<bool(Simulator const&)> halt;
    //! \brief C function Reacting to external events (refered by their ID for
    //! example keyboard keys).
    std::function<void(Simulator& simulator, size_t key)> react;
};

// ****************************************************************************
//! \brief Simulation class configured when the simulation file (shared library)
//! is loaded.
// ****************************************************************************
class Simulation: public Scenario, public DynamicLoader
{
private:

    //--------------------------------------------------------------------------
    //! \brief Concrete implementation of the callback triggered when the
    //! simulation file (shared library) has been loaded with success. This
    //! method will loaded desired C functions. In case of error an execption is
    //! thrown.
    //--------------------------------------------------------------------------
    virtual void onLoaded() override;
};

#endif
