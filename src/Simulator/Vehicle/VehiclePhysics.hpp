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

#  include "Vehicle/VehicleShape.hpp"
#  include "Vehicle/VehicleControl.hpp"
#  include <SFML/System/Vector2.hpp>

// *****************************************************************************
//! \brief
// *****************************************************************************
template<class BLUEPRINT>
class VehiclePhysics
{
public:

    //--------------------------------------------------------------------------
    VehiclePhysics(VehicleShape<BLUEPRINT> const& shape, VehicleControl const& control)
        : m_shape(shape), m_control(control)
    {}

    //--------------------------------------------------------------------------
    virtual ~VehiclePhysics() = default;

    //--------------------------------------------------------------------------
    //! \brief Set initial values needed by tricycle kinematic equations.
    //! \param[in] position: the (x, y) world coordinated of the car is the
    //!   middle of the rear axle.
    //! \param[in] acceleration: initial longitudinal acceleration [meter /
    //!   second^2].
    //! \param[in] speed: initial longitudinal speed [meter / second].
    //! \param[in] position: initial world position of the car (its center of
    //!   the rear axle) [meter].
    //! \param[in] heading: the initial yaw angle of the vehicle [radian].
    //--------------------------------------------------------------------------
    virtual void init(float acceleration, float speed, sf::Vector2f position, float heading)
    {
        m_acceleration = acceleration;
        m_speed = speed;
        m_position = position;
        m_heading = heading;
    }

    //--------------------------------------------------------------------------
    //! \brief Update discrete time equations from continuous time equations
    //! described in these pictures:
    //! ../../doc/pics/TricycleVehicle.png
    //! ../../doc/pics/TricycleKinematicEq.png
    //! in where:
    //!  - L is the vehicle wheelbase [meter].
    //!  - v is the vehicle longitudinal speed [meter / second].
    //!  - theta is the car heading (yaw) [radian].
    //!  - delta is the steering angle [radian].
    //--------------------------------------------------------------------------
    virtual void update(float const dt) = 0;

    //--------------------------------------------------------------------------
    //! \brief Const getter: return longitudinal acceleration [meter/second^2].
    //--------------------------------------------------------------------------
    inline float acceleration() const
    {
        return m_acceleration;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the longitudinal speed [meter/second].
    //--------------------------------------------------------------------------
    inline float speed() const
    {
        return m_speed;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the position of the middle of the rear axle
    //! inside the world coordinates.
    //--------------------------------------------------------------------------
    inline sf::Vector2f position() const
    {
        return m_position;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading (yaw angle) [rad].
    //--------------------------------------------------------------------------
    inline float heading() const
    {
        return m_heading;
    }

protected:

    //! \brief 
    VehicleShape<BLUEPRINT> const& m_shape;
    //! \brief 
    VehicleControl const& m_control;
    //! \brief Longitudinal acceleration [meter / second^2].
    float m_acceleration;
    //! \brief Longitudinal speed [meter / second].
    float m_speed;
    //! \brief World position of the car (its center of the rear axle) [meter].
    sf::Vector2f m_position;
    //! \brief Yaw angle of the vehicle [radian].
    float m_heading;
};

#endif
