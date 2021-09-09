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

#ifndef CAR_SHAPE_HPP
#  define CAR_SHAPE_HPP

#  include "CarDimension.hpp"
#  include "Utils.hpp"
//#  include <SFML/System/Vector2.hpp>
#  include <SFML/Graphics/Rect.hpp>
#  include <SFML/Graphics/RectangleShape.hpp>
#  include <array>

struct Wheel
{
    //! \brief Relative position from
    sf::Vector2f offset;
    //! \brief current position in the world
    sf::Vector2f position;
    //! \brief yaw angle
    float steering;
    //! \brief speed
    //float speed;

    friend std::ostream& operator<<(std::ostream& os, Wheel const& wheel)
    {
        return os << "{ position=(" << wheel.position.x
                  << ", " << wheel.position.y
                  << "), steering=" << RAD2DEG(wheel.steering)
                  << " }";
    }
};


// Position, orientation and wheel positions
template<long unsigned int N>
class VehicleShape
{
public:

    virtual ~VehicleShape() = default;

    inline float heading() const
    {
        return m_heading;
    }

    inline sf::Vector2f position() const
    {
        return m_position;
    }

    inline std::array<Wheel, N> const& wheels() const
    {
        return m_wheels;
    }

    inline float steering() const
    {
        return m_wheels[0].steering;
    }

    sf::FloatRect boundinBox() const
    {
        return m_obb.getGlobalBounds();
    }

    bool intersects(VehicleShape<N> const& other) const
    {
        return boundinBox().intersects(other.boundinBox());
    }

    virtual void steering(float const v) = 0;

    friend std::ostream& operator<<(std::ostream& os, VehicleShape const& shape)
    {
        os << "  Shape {" << std::endl
           << "    position=(" << shape.m_position.x << ", " << shape.m_position.y << ")," << std::endl
           << "    heading=" << RAD2DEG(shape.m_heading) << std::endl;

        auto i = N;
        while (i--)
        {
            os << "    wheel[" << i << "]=" << shape.m_wheels[i] << "," << std::endl;
        }
        return os << "  }";
    }

public:

    sf::RectangleShape m_obb;

protected:

    sf::Vector2f m_position;
    //! \brief Oriented bounding box for collision
    float m_heading;
    std::array<Wheel, N> m_wheels;
};


class TrailerShape: public VehicleShape<2>
{
public:

    enum WheelType { RR, RL };

    TrailerShape(TrailerDimension const& dim_)
        : dim(dim_)
    {
        // Origin on the middle of the rear wheels
        m_obb.setSize(sf::Vector2f(dim.length, dim.width));
        m_obb.setOrigin(dim.back_overhang, m_obb.getSize().y / 2);

        // Wheel offset along the Y-axis
        const float K = dim.width / 2 - dim.wheel_width / 2;

        m_wheels[WheelType::RL].offset = sf::Vector2f(0.0f, K);
        m_wheels[WheelType::RR].offset = sf::Vector2f(0.0f, -K);
        m_wheels[WheelType::RL].steering = m_wheels[WheelType::RR].steering = 0.0f;
        //wheels[FL].speed = m_wheels[FR].speed = NAN;
        //wheels[RL].speed = m_wheels[RR].speed = NAN;
    }

    void set(sf::Vector2f const& position, float const heading)
    {
        m_heading = heading;
        m_position = position;
        m_obb.setPosition(position);
        m_obb.setRotation(RAD2DEG(heading));

        m_wheels[WheelType::RL].position = position + ROTATE(m_wheels[WheelType::RL].offset, heading);
        m_wheels[WheelType::RR].position = position + ROTATE(m_wheels[WheelType::RR].offset, heading);
    }

    inline Wheel const& wheel(WheelType const i) const { return m_wheels[i]; }

    virtual void steering(float const) override
    {
        // Do nothing
    }

public:

    TrailerDimension const dim;
};




class CarShape: public VehicleShape<4>
{
public:

    enum WheelType { FL, FR, RR, RL };

    CarShape(CarDimension const& dim_)
        : dim(dim_)
    {
        // Origin on the middle of the rear wheels
        m_obb.setSize(sf::Vector2f(dim.length, dim.width));
        m_obb.setOrigin(dim.back_overhang, m_obb.getSize().y / 2);

        // Wheel offset along the Y-axis
        const float K = dim.width / 2 - dim.wheel_width / 2;

        m_wheels[WheelType::FL].offset = sf::Vector2f(dim.wheelbase, -K);
        m_wheels[WheelType::FR].offset = sf::Vector2f(dim.wheelbase, K);
        m_wheels[WheelType::RL].offset = sf::Vector2f(0.0f, -K);
        m_wheels[WheelType::RR].offset = sf::Vector2f(0.0f, K);
        //wheels[FL].speed = m_wheels[FR].speed = NAN;
        //wheels[RL].speed = m_wheels[RR].speed = NAN;
    }

    void set(sf::Vector2f const& position, float const heading, float const steering)
    {
        m_heading = heading;
        m_position = position;
        m_obb.setPosition(position);
        m_obb.setRotation(RAD2DEG(heading));

        m_wheels[WheelType::FL].steering = m_wheels[WheelType::FR].steering = steering;
        m_wheels[WheelType::RL].steering = m_wheels[WheelType::RR].steering = 0.0f;
        m_wheels[WheelType::FL].position = position + ROTATE(m_wheels[WheelType::FL].offset, heading);
        m_wheels[WheelType::FR].position = position + ROTATE(m_wheels[WheelType::FR].offset, heading);
        m_wheels[WheelType::RL].position = position + ROTATE(m_wheels[WheelType::RL].offset, heading);
        m_wheels[WheelType::RR].position = position + ROTATE(m_wheels[WheelType::RR].offset, heading);
    }

    inline Wheel const& wheel(WheelType const i) const { return m_wheels[i]; }

    virtual void steering(float const v) override
    {
        m_wheels[WheelType::FL].steering = m_wheels[WheelType::FR].steering = v;
    }

public:

    CarDimension const dim;
};

#endif
