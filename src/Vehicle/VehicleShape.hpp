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

#ifndef VEHICLE_SHAPE_HPP
#  define VEHICLE_SHAPE_HPP

#  include "Utils/Collide.hpp"
#  include "Vehicle/VehicleDimension.hpp"
#  include "Vehicle/Wheels.hpp"
#  include "Sensors/Radar.hpp"
#  include <iostream>

// Ackermann steering mechanics
// TODO https://datagenetics.com/blog/december12016/index.html
// TODO https://www.researchgate.net/publication/349289743_Designing_Variable_Ackerman_Steering_Geometry_for_Formula_Student_Race_Vehicle/link/6028146aa6fdcc37a824e404/download

// *****************************************************************************
//! \brief A vehicle shape knows it position, heading, bounding box and the
//! blueprint of its wheels.
//! \tparam Dim: a class holding dimension of the shape (ie TrailerDimension,
//! CarDimension ...)
// *****************************************************************************
template<class Dim>
class VehicleShape
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor with the vehicle dimension (ie
    //! TrailerDimension, CarDimension ...)
    //--------------------------------------------------------------------------
    VehicleShape(Dim const& dimension)
        : dim(dimension)
    {}

    //--------------------------------------------------------------------------
    //! \brief Because of virtual methods.
    //--------------------------------------------------------------------------
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
    //! \brief Const getter: return the const reference of the container holding
    //! all the wheels.
    //--------------------------------------------------------------------------
    inline std::vector<Wheel> const& wheels() const
    {
        return m_wheels;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the const reference of the nth wheel.
    //--------------------------------------------------------------------------
    inline Wheel const& wheel(size_t const nth) const
    {
        assert(nth < m_wheels.size());
        return m_wheels[nth];
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the steering angle of the desired wheel.
    //--------------------------------------------------------------------------
    inline float steering(size_t const nth) const
    {
        assert(nth < m_wheels.size());
        return m_wheels[nth].steering;
    }

    //--------------------------------------------------------------------------
    //! \brief const getter: return the oriented bounding box (OBB) of the shape.
    //--------------------------------------------------------------------------
    inline sf::RectangleShape const& obb() const
    {
        return m_obb;
    }

    //--------------------------------------------------------------------------
    //! \brief Check if the shape collides with another vehicle shape.
    //! \param[out] p: return the minimum translation vector in case of collision
    //! \return true in case of collision and return the position of the collision.
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
    TrailerShape(TrailerDimension const& dimension);

    //--------------------------------------------------------------------------
    //! \brief Set to shape its new attitude
    //--------------------------------------------------------------------------
    void set(sf::Vector2f const& position, float const heading);
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
    CarShape(CarDimension const& dimension);

    //--------------------------------------------------------------------------
    //! \brief Set to shape its new attitude
    //--------------------------------------------------------------------------
    void set(sf::Vector2f const& position, float const heading, float const steering);

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    std::vector<SensorShape>& sensors()
    {
        return m_sensor_shapes;
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    std::vector<SensorShape> const& sensors() const
    {
        return m_sensor_shapes;
    }

private:

    //! \brief Information on the sensors
    std::vector<SensorShape> m_sensor_shapes;
};

#endif
