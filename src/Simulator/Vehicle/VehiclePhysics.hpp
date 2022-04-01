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

#ifndef VEHICLE_PHYSICS_HPP
#  define VEHICLE_PHYSICS_HPP

#  include "Simulator/Movable.hpp"
#  include "Simulator/Vehicle/VehicleShape.hpp"
#  include "Simulator/Vehicle/VehicleControl.hpp"

// *****************************************************************************
//! \brief Base class for computing the vhecile physic (kinematic, dynamics ...)
//! need to implement void Movable::update(float const dt)
// *****************************************************************************
template<class BLUEPRINT>
class VehiclePhysics : public Movable
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor: aggregation of the vehicle shape and its
    //! control.
    //--------------------------------------------------------------------------
    VehiclePhysics(VehicleShape<BLUEPRINT> const& shape, VehicleControl const& control)
        : m_shape(shape), m_control(control)
    {}

    //--------------------------------------------------------------------------
    //! \brief Needed beause of virtual methods.
    //--------------------------------------------------------------------------
    virtual ~VehiclePhysics() = default;

protected:

    //! \brief
    VehicleShape<BLUEPRINT> const& m_shape;
    //! \brief
    VehicleControl const& m_control;
};

#endif
