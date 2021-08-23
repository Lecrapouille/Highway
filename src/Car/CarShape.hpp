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
#  include <SFML/System/Vector2.hpp>
#  include <array>

struct Wheel
{
    enum Type { FL, FR, RR, RL };

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

class CarShape
{
public:

    CarShape(CarDimension const& dim_)
        : dim(dim_)
    {
        // Wheel offset along the Y-axis
        const float K = dim.width / 2 - dim.wheel_width / 2;

        m_wheels[Wheel::Type::FL].offset = sf::Vector2f(dim.wheelbase, K);
        m_wheels[Wheel::Type::FR].offset = sf::Vector2f(dim.wheelbase, -K);
        m_wheels[Wheel::Type::RL].offset = sf::Vector2f(0.0f, K);
        m_wheels[Wheel::Type::RR].offset = sf::Vector2f(0.0f, -K);
        //wheels[FL].speed = m_wheels[FR].speed = NAN;
        //wheels[RL].speed = m_wheels[RR].speed = NAN;
    }

    void set(sf::Vector2f const& position, float const heading, float const steering)
    {
        m_heading = heading;
        m_position = position;

        const float COS_YAW = cosf(heading);
        const float SIN_YAW = sinf(heading);
        const float FRONT_X = position.x + m_wheels[Wheel::Type::FL].offset.x * COS_YAW;
        const float FRONT_Y = position.y + m_wheels[Wheel::Type::FL].offset.x * SIN_YAW;
        const float OFFSET_SIN = m_wheels[Wheel::Type::FL].offset.y * SIN_YAW;
        const float OFFSET_COS = m_wheels[Wheel::Type::FL].offset.y * COS_YAW;

        m_wheels[Wheel::Type::FL].position.x = FRONT_X - OFFSET_SIN;
        m_wheels[Wheel::Type::FL].position.y = FRONT_Y + OFFSET_COS;
        m_wheels[Wheel::Type::FR].position.x = FRONT_X + OFFSET_SIN;
        m_wheels[Wheel::Type::FR].position.y = FRONT_Y - OFFSET_COS;

        m_wheels[Wheel::Type::RL].position.x = position.x + OFFSET_SIN;
        m_wheels[Wheel::Type::RL].position.y = position.y - OFFSET_COS;
        m_wheels[Wheel::Type::RR].position.x = position.x - OFFSET_SIN;
        m_wheels[Wheel::Type::RR].position.y = position.y + OFFSET_COS;

        m_wheels[Wheel::Type::FL].steering = m_wheels[Wheel::Type::FR].steering = steering;
        m_wheels[Wheel::Type::RL].steering = m_wheels[Wheel::Type::RR].steering = 0.0f;
    }

    inline std::array<Wheel, 4> const& wheels() const { return m_wheels; }
    inline Wheel const& wheel(Wheel::Type const i) const { return m_wheels[i]; }
    inline float heading() const { return m_heading; }
    inline float steering() const
    {
        return m_wheels[0].steering;
    }

    void steering(float const v)
    {
        m_wheels[Wheel::Type::FL].steering = m_wheels[Wheel::Type::FR].steering = v;
    }

    inline sf::Vector2f position() const
    {
        return m_position;
    }

    friend std::ostream& operator<<(std::ostream& os, CarShape const& shape)
    {
        return os << "  Shape {" << std::endl
                  << "    position=(" << shape.m_position.x << ", " << shape.m_position.y << ")," << std::endl
                  << "    heading=" << RAD2DEG(shape.m_heading) << std::endl
                  << "    wheel[FL]=" << shape.m_wheels[Wheel::Type::FL] << "," << std::endl
                  << "    wheel[FR]=" << shape.m_wheels[Wheel::Type::FR] << "," << std::endl
                  << "    wheel[RL]=" << shape.m_wheels[Wheel::Type::RL] << "," << std::endl
                  << "    wheel[RR]=" << shape.m_wheels[Wheel::Type::RR] << std::endl
                  << "  }";
    }

public:

    CarDimension const dim;

protected:

    std::array<Wheel, 4> m_wheels;
    sf::Vector2f m_position;
    float m_heading;
};

#endif
