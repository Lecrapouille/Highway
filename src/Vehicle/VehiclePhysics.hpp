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
//! \brief Base class for doing kinematic or dynamic on a linked list of
//! elements.
//! \tparam S a VehicleShape
// *****************************************************************************
class IPhysics
{
public:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    IPhysics(std::string const& n)
        : name(n)
    {}

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual ~IPhysics() = default;

    //--------------------------------------------------------------------------
    //! \brief Join with the given front vehicle.
    //! \param[inout] physics: the front vehicle.
    //--------------------------------------------------------------------------
    void attachTo(IPhysics& front)
    {
        std::cout << "Attaching " << name << " to " << front.name << std::endl;
        assert(this != &front);
        next = &front;
        front.previous = this;
    }

    //--------------------------------------------------------------------------
    //! \brief Update the physical model.
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
    //! \brief Const getter: return longitudinal acceleration [meter/seconds^2].
    //--------------------------------------------------------------------------
    inline float acceleration() const
    {
        return m_acceleration;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return longitudinal speed [meter/seconds].
    //--------------------------------------------------------------------------
    inline float speed() const
    {
        return m_speed;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return position of the middle of the rear axle.
    //--------------------------------------------------------------------------
    virtual sf::Vector2f position() const = 0;

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading [rad].
    //--------------------------------------------------------------------------
    virtual float heading() const = 0;

private:

    virtual void onUpdate(CarControl const& control, float const dt) = 0;

public:

    std::string const& name;
    //! \brief Next joined trailer or vehicle. Please do not manage memory:
    //! this pointer is just a reference.
    IPhysics* next = nullptr;
    //! \brief Previous joined trailer or vehicle. Please do not manage memory:
    //! this pointer is just a reference.
    IPhysics* previous = nullptr;

protected:

    float m_speed = 0.0f;
    float m_acceleration = 0.0f;
    float m_heading;
};

// *****************************************************************************
//! \brief
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
    //! \brief
    //--------------------------------------------------------------------------
    void init(float const speed, float const heading);

    //--------------------------------------------------------------------------
    //! \brief Const getter: return position of the middle of the rear axle.
    //--------------------------------------------------------------------------
    virtual inline sf::Vector2f position() const override
    {
        return m_shape.position();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading [rad].
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
    //! \brief
    //--------------------------------------------------------------------------
    virtual void onUpdate(CarControl const& control, float const dt) override;

protected:

    TrailerShape& m_shape;
};

// *****************************************************************************
//! \brief
// *****************************************************************************
class CarKinematic: public IPhysics
{
public:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    CarKinematic(std::string const& n, CarShape& shape)
        : IPhysics(n), m_shape(shape)
    {}

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual ~CarKinematic() = default;

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void init(sf::Vector2f const& position, float const heading, float const speed,
              float const steering);

    //--------------------------------------------------------------------------
    //! \brief Const getter: return position of the middle of the rear axle.
    //--------------------------------------------------------------------------
    virtual inline sf::Vector2f position() const override
    {
        return m_shape.position();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading [rad].
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
    //! \brief
    //--------------------------------------------------------------------------
    virtual void onUpdate(CarControl const& control, float const dt) override;

protected:

    CarShape& m_shape;
};

#endif
