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

#ifndef CAR_SHAPE_HPP
#  define CAR_SHAPE_HPP

#  include "Utils/Collide.hpp"
#  include "Car/CarDimension.hpp"
#  include "Car/Wheels.hpp"
#  include "Sensors/Radar.hpp"
#  include <iostream>

// TODO https://datagenetics.com/blog/december12016/index.html
// TODO https://www.researchgate.net/publication/349289743_Designing_Variable_Ackerman_Steering_Geometry_for_Formula_Student_Race_Car/link/6028146aa6fdcc37a824e404/download

// *****************************************************************************
//! \brief A vehicle shape knows it position, heading, bounding box and position
//! of wheels.
// *****************************************************************************
template<class Dim>
class VehicleShape
{
public:

    VehicleShape(Dim const& dimension)
        : dim(dimension)
    {}

    virtual ~VehicleShape() = default;

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading [rad].
    //--------------------------------------------------------------------------
    inline float heading() const
    {
        return DEG2RAD(m_obb.getRotation());
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return position of the middle of the rear axle.
    //--------------------------------------------------------------------------
    inline sf::Vector2f position() const
    {
        return m_obb.getPosition();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return all the wheels
    //--------------------------------------------------------------------------
    inline std::vector<Wheel> const& wheels() const
    {
        return m_wheels;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the nth wheel
    //--------------------------------------------------------------------------
    inline Wheel const& wheel(size_t const nth) const
    {
        assert(nth < m_wheels.size());
        return m_wheels[nth];
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the steering angle of the desired wheel
    //--------------------------------------------------------------------------
    inline float steering(size_t const nth) const
    {
        assert(nth < m_wheels.size());
        return m_wheels[nth].steering;
    }

    //--------------------------------------------------------------------------
    //! \brief const getter: return the oriented bounding box (obb) of the shape.
    //--------------------------------------------------------------------------
    inline sf::RectangleShape const& obb() const
    {
        return m_obb;
    }

    //--------------------------------------------------------------------------
    //! \brief Check if the shape collides with another vehicle shape.
    //! \param[out] p: return the minimum translation vector in case of collision
    //! \return true in case of collision and return the
    //--------------------------------------------------------------------------
    inline bool collides(VehicleShape const& other, sf::Vector2f& p) const
    {
        return collide(obb(), other.obb(), p);
    }

    //--------------------------------------------------------------------------
    //! \brief Debug purpose only: show shape information.
    //--------------------------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, VehicleShape const& shape)
    {
        os << "  Shape {" << std::endl << "    position=("
           << shape.position().x << ", " << shape.position().y << "),"
           << std::endl
           << "    heading=" << RAD2DEG(shape.heading())
           << std::endl;

        auto i = shape.wheels().size();
        while (i--)
        {
            os << "    wheel[" << i << "]=" << shape.wheel(i) << "," << std::endl;
        }
        return os << "  }";
    }

public:

    //! \brief Const reference to the vehicle's dimension.
    Dim const& dim;

protected:

    //! \brief Oriented bounding box for attitude and collision
    sf::RectangleShape m_obb;
    //! \brief Information on the wheels
    std::vector<Wheel> m_wheels;
};

// *****************************************************************************
//! \brief A vehicle shape specialized for trailer with 2 wheels.
// *****************************************************************************
class TrailerShape: public VehicleShape<TrailerDimension>
{
public:

    //--------------------------------------------------------------------------
    //! \brief Wheel's names: RR: rear right, RL: rear left.
    //--------------------------------------------------------------------------
    enum WheelName { RR, RL };

    //--------------------------------------------------------------------------
    //! \brief Default constructor: trailer shape with its dimension
    //--------------------------------------------------------------------------
    TrailerShape(TrailerDimension const& dimension)
        : VehicleShape(dimension)
    {
        // 2 wheels
        m_wheels.resize(2);

        // Origin on the middle of the rear wheel axle
        m_obb.setSize(sf::Vector2f(dim.length, dim.width));
        m_obb.setOrigin(dim.back_overhang, m_obb.getSize().y / 2);

        // Offset along the rear axle
        const float K = dim.width / 2 - dim.wheel_width / 2;
        m_wheels[WheelName::RL].offset = sf::Vector2f(0.0f, K);
        m_wheels[WheelName::RR].offset = sf::Vector2f(0.0f, -K);

        m_wheels[WheelName::RL].steering = 0.0f;
        m_wheels[WheelName::RR].steering = 0.0f;

        //m_wheels[WheelName::RL].speed = NAN;
        //m_wheels[WheelName::RR].speed = NAN;
    }

    //--------------------------------------------------------------------------
    //! \brief Set to shape its new attitude
    //--------------------------------------------------------------------------
    void set(sf::Vector2f const& position, float const heading)
    {
        m_obb.setPosition(position);
        m_obb.setRotation(RAD2DEG(heading));

        m_wheels[WheelName::RL].position =
                position + ROTATE(m_wheels[WheelName::RL].offset, heading);
        m_wheels[WheelName::RR].position =
                position + ROTATE(m_wheels[WheelName::RR].offset, heading);
    }
};

// *****************************************************************************
//! \brief A vehicle shape specialized for car with 4 wheels.
// *****************************************************************************
class CarShape: public VehicleShape<CarDimension>
{
public:

    //--------------------------------------------------------------------------
    //! \brief Wheel's names: FL: front left, FR: front rigeht, RR: rear right,
    //! RL: rear left.
    //--------------------------------------------------------------------------
    enum WheelName { FL, FR, RR, RL };

    //--------------------------------------------------------------------------
    //! \brief Default constructor: car shape with its dimension
    //--------------------------------------------------------------------------
    CarShape(CarDimension const& dimension)
        : VehicleShape(dimension)
    {
        // Origin on the middle of the rear wheel axle
        m_obb.setSize(sf::Vector2f(dim.length, dim.width));
        m_obb.setOrigin(dim.back_overhang, m_obb.getSize().y / 2);

        // 4 wheels
        m_wheels.resize(4);

        // Offset along the rear axle
        const float K = dim.width / 2 - dim.wheel_width / 2;
        m_wheels[WheelName::FL].offset = sf::Vector2f(dim.wheelbase, -K); // FIXME L and R inversed
        m_wheels[WheelName::FR].offset = sf::Vector2f(dim.wheelbase, K);
        m_wheels[WheelName::RL].offset = sf::Vector2f(0.0f, -K);
        m_wheels[WheelName::RR].offset = sf::Vector2f(0.0f, K);

        m_wheels[WheelName::FL].steering = 0.0f;
        m_wheels[WheelName::FR].steering = 0.0f;
        m_wheels[WheelName::RR].steering = 0.0f;
        m_wheels[WheelName::RL].steering = 0.0f;

        //wheels[FL].speed = m_wheels[FR].speed = NAN;
        //wheels[RL].speed = m_wheels[RR].speed = NAN;

        // 4 radars: 1 one each wheel (to make simple)
        m_sensor_shapes.resize(1/*4*/);
        //m_sensor_shapes[WheelName::FL].orientation = -90.0f;
        //m_sensor_shapes[WheelName::FR].orientation = 90.0f;
        m_sensor_shapes[0*WheelName::RL].orientation = -90.0f;
        //m_sensor_shapes[WheelName::RR].orientation = 90.0f;

        //m_sensor_shapes[WheelName::FL].offset = sf::Vector2f(dim.wheelbase, -K);
        //m_sensor_shapes[WheelName::FR].offset = sf::Vector2f(dim.wheelbase, K);
        m_sensor_shapes[0*WheelName::RL].offset = sf::Vector2f(0.0f, -K);
        //m_sensor_shapes[WheelName::RR].offset = sf::Vector2f(0.0f, K);
    }

    //--------------------------------------------------------------------------
    //! \brief Set to shape its new attitude
    //--------------------------------------------------------------------------
    void set(sf::Vector2f const& position, float const heading, float const steering)
    {
        m_obb.setPosition(position);
        m_obb.setRotation(RAD2DEG(heading));

        m_wheels[WheelName::FL].position =
                position + ROTATE(m_wheels[WheelName::FL].offset, heading);
        m_wheels[WheelName::FR].position =
                position + ROTATE(m_wheels[WheelName::FR].offset, heading);
        m_wheels[WheelName::RR].position =
                position + ROTATE(m_wheels[WheelName::RR].offset, heading);
        m_wheels[WheelName::RL].position =
                position + ROTATE(m_wheels[WheelName::RL].offset, heading);

        m_wheels[WheelName::FL].steering = steering;
        m_wheels[WheelName::FR].steering = steering;

        for (auto& it: m_sensor_shapes)
        {
           it.obb.setRotation(it.orientation + RAD2DEG(heading));
           it.obb.setPosition(position + ROTATE(it.offset, heading));
        }
    }

    std::vector<SensorShape>& sensors()
    {
        return m_sensor_shapes;
    }

    std::vector<SensorShape> const& sensors() const
    {
        return m_sensor_shapes;
    }

private:

    //! \brief Information on the sensors
    std::vector<SensorShape> m_sensor_shapes;
};

#endif
