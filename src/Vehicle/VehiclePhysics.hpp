// 2021 Quentin Quadrat quentin.quadrat@gmail.com
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

#  include "Vehicle/VehicleControl.hpp"
#  include "Vehicle/VehicleShape.hpp"
#  include <string>

// *****************************************************************************
//! \brief Base class for computing a set of linked vehicle kinematic or vehicle
//! dynamic equations.
//! For example: you can implement on derived class:
//! - Bicycle kinematics:
//!   - if the desired position is located at the center of gravity:
//!     ../../doc/pics/front_axle_bycicle_model.png
//!   - if the desired position is located at the middle of the front axle:
//!     ../../doc/pics/cg_bycicle_model.png
//!   - if the desired position is located at the middle of the rear axle:
//!     ../../doc/pics/TricycleKinematicEq.png
//! - Car dynamics:
//!   - https://github.com/quangnhat185/Self-driving_cars_toronto_coursera
//! - Trailer kinematics:
//!   - "Flatness and motion Planning: the Car with n-trailers" by Pierre Rouchon
//! Linked list of vehicle physics is mainly for attaching trailers.
//! \tparam S a VehicleShape
// *****************************************************************************
class IPhysics
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor.
    //! \param[in] n: the name of the vehicle (debug purpose only).
    //--------------------------------------------------------------------------
    IPhysics(std::string const& n)
        : name(n)
    {}

    //--------------------------------------------------------------------------
    //! \brief Needed because of virtual methods.
    //--------------------------------------------------------------------------
    virtual ~IPhysics() = default;

    //--------------------------------------------------------------------------
    //! \brief Join with the given front vehicle.
    //! \param[inout] front: the front vehicle physic.
    //--------------------------------------------------------------------------
    void attachTo(IPhysics& front)
    {
        std::cout << "Attaching " << name << " to " << front.name << std::endl;
        assert(this != &front);
        next = &front;
        front.previous = this;
    }

    //--------------------------------------------------------------------------
    //! \brief Base method updating the whole link of physical vehicles.
    //! Call the onUpdate() method to really update physic equations.
    //! \param[in] control: the cruise control of the head vehicle.
    //! \param[in] dt: delta time [seconds] from the previous call.
    //--------------------------------------------------------------------------
    void update(CarControl const& control, float const dt)
    {
        onUpdate(control, dt);
        if (previous != nullptr)
        {
            previous->update(control, dt);
        }
    }

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
    virtual sf::Vector2f position() const = 0;

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading (yaw angle) [rad].
    //--------------------------------------------------------------------------
    virtual float heading() const = 0;

private:

    //--------------------------------------------------------------------------
    //! \brief Virtual method updating vehicle physics equations.
    //! \param[in] control: the cruise control of the head vehicle.
    //! \param[in] dt: delta time [seconds] from the previous call.
    //--------------------------------------------------------------------------
    virtual void onUpdate(CarControl const& control, float const dt) = 0;

public:

    //! \brief Vehicle name (debug purpose only).
    std::string const& name;
    //! \brief Next joined trailer or vehicle. Please do not manage memory:
    //! this pointer is just a reference.
    IPhysics* next = nullptr;
    //! \brief Previous joined trailer or vehicle. Please do not manage memory:
    //! this pointer is just a reference.
    IPhysics* previous = nullptr;

protected:

    //! \brief Longitudinal speed [meter / second]
    float m_speed = 0.0f;
    //! \brief Longitudinal acceleration [meter / second / second]
    float m_acceleration = 0.0f;
    //! \brief Vehicle yaw angle [radian]
    float m_heading;
};

// *****************************************************************************
//! \brief Class computing the kinematic equation of a trailer.
//! A trailer can be attached to another trailer or to a tractor vehicle.
// *****************************************************************************
class TrailerKinematic: public IPhysics
{
public:

    //--------------------------------------------------------------------------
    //! \brief Attach this trailer to another front vehicle (trailer or car).
    //! \param[in] dim: the trailer dimension.
    //! \param[inout] front: the front vehicle.
    //--------------------------------------------------------------------------
    TrailerKinematic(std::string const& n, TrailerShape& shape, IPhysics& front)
        : IPhysics(n), m_shape(shape)
    {
        attachTo(front);
    }

    //--------------------------------------------------------------------------
    //! \brief Set initial values needed by trailer kinematic equations.
    //! \note the position: the (x, y) world coordinated of the middle of the
    //! rear axle is deduced.
    //! \param[in] heading: the initial yaw angle of the vehicle [radian]
    //! \param[in] speed: initial longitudinal speed [meter / second].
    //--------------------------------------------------------------------------
    void init(float const speed, float const heading);

    //--------------------------------------------------------------------------
    //! \brief Const getter: return position of the middle of the rear axle
    //! inside the world coordinates.
    //--------------------------------------------------------------------------
    virtual inline sf::Vector2f position() const override
    {
        return m_shape.position();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading (yaw angle) [rad].
    //--------------------------------------------------------------------------
    virtual inline float heading() const override
    {
        return m_shape.heading();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the shape.
    //--------------------------------------------------------------------------
    inline TrailerShape const& shape() const
    {
        return m_shape;
    }

private:

    //--------------------------------------------------------------------------
    //! \brief Concrete method updating vehicle physics equations.
    //! See "Flatness and motion Planning: the Car with n-trailers" by Pierre Rouchon ...
    //! \param[in] control: the cruise control of the head vehicle.
    //! \param[in] dt: delta time [seconds] from the previous call.
    //--------------------------------------------------------------------------
    virtual void onUpdate(CarControl const& control, float const dt) override;

protected:

    TrailerShape& m_shape;
};

// *****************************************************************************
//! \brief Simple car kinematic using the tricycle kinematic equations.
//! The position (x, y) of the car is the middle of the rear axle.
//! ../../doc/pics/TricycleVehicle.png
//! ../../doc/pics/TricycleKinematicEq.png
// *****************************************************************************
class TricycleKinematic: public IPhysics
{
public:

    //--------------------------------------------------------------------------
    //! \brief default constructor: define a name and a shape.
    //! \param[in] name: the name of the vehicle (debug purpose only).
    //! \param[in] shape: vahicle dimension.
    //--------------------------------------------------------------------------
    TricycleKinematic(std::string const& name, CarShape& shape)
        : IPhysics(name), m_shape(shape)
    {}

    //--------------------------------------------------------------------------
    //! \brief Needed because of virtual methods.
    //--------------------------------------------------------------------------
    virtual ~TricycleKinematic() = default;

    //--------------------------------------------------------------------------
    //! \brief Set initial values needed by tricycle kinematic equations.
    //! \param[in] position: the (x, y) world coordinated of the car is the
    //! middle of the rear axle.
    //! \param[in] heading: the initial yaw angle of the vehicle [radian]
    //! \param[in] speed: initial longitudinal speed [meter / second].
    //! \param[in] steering: initial steering angle [radina].
    //--------------------------------------------------------------------------
    void init(sf::Vector2f const& position, float const heading, float const speed,
              float const steering);

    //--------------------------------------------------------------------------
    //! \brief Const getter: return position of the middle of the rear axle
    //! inside the world coordinates.
    //--------------------------------------------------------------------------
    virtual inline sf::Vector2f position() const override
    {
        return m_shape.position();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading (yaw angle) [rad].
    //--------------------------------------------------------------------------
    virtual inline float heading() const override
    {
        return m_shape.heading();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the shape.
    //--------------------------------------------------------------------------
    inline CarShape const& shape() const
    {
        return m_shape;
    }

private:

    //--------------------------------------------------------------------------
    //! \brief Update discrete time equations from continuous time equations
    //! described in these pictures:
    //! ../../doc/pics/TricycleVehicle.png
    //! ../../doc/pics/TricycleKinematicEq.png
    // in where:
    //  - L is the vehicle wheelbase [meter]
    //  - v is the vehicle longitudinal speed [meter / second]
    //  - theta is the car heading (yaw) [radian]
    //  - delta is the steering angle [radian]
    //--------------------------------------------------------------------------
    virtual void onUpdate(CarControl const& control, float const dt) override;

protected:

    CarShape& m_shape;
};

#endif
