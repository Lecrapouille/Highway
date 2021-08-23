// 2021 Quentin Quadrat lecrapouille@gmail.com
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

#ifndef CAR_PHYSICS_HPP
#  define CAR_PHYSICS_HPP

#  include "CarControl.hpp"
#  include "CarShape.hpp"
#  include <iostream>
//#  include <SFML/System/Vector2.hpp>

class CarPhysics
{
public:

    virtual ~CarPhysics() = default;
    virtual void update(float const dt, CarControl const& control) = 0;
    virtual float acceleration() const = 0;
    virtual float speed() const = 0;
    virtual sf::Vector2f position() const = 0;
};

class CarKinematic: public CarPhysics
{
public:

    CarKinematic(CarDimension const& dim)
        : m_shape(dim)
    {}

    void init(sf::Vector2f const& position, float const speed,
              float const heading, float const steering)
    {
        m_shape.set(position, heading, steering);
        m_speed = speed;
    }

    virtual void update(float const dt, CarControl const& control) override
    {
        float steering = control.outputs.steering;
        float heading = m_shape.heading();
        sf::Vector2f position = m_shape.position();

        m_speed = control.outputs.body_speed;
        heading += dt * m_speed * tanf(steering) / m_shape.dim.wheelbase;
        position.x += dt * m_speed * cosf(heading);
        position.y += dt * m_speed * sinf(heading);
        m_shape.set(position, heading, steering);
    }

    virtual float acceleration() const override
    {
        return 0.0f;
    }

    virtual float speed() const override
    {
        return m_speed;
    }

    virtual sf::Vector2f position() const override
    {
        return m_shape.position();
    }

    CarShape const& shape() const
    {
        return m_shape;
    }

private:

    CarShape m_shape;
    float m_speed = 0.0f;
};

#endif
