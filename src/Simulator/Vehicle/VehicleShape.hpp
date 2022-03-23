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

#ifndef VEHICLESHAPE_HPP
#  define VEHICLESHAPE_HPP

#  include "Math/Collide.hpp"
#  include "Math/Math.hpp"
#  include "Sensors/SensorShape.hpp"
#  include <vector>
#  include <cassert>
#  include <memory>

// *****************************************************************************
//! \brief
// *****************************************************************************
template<class BLUEPRINT>
class VehicleShape
{
public:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    VehicleShape(BLUEPRINT& blueprint/*, std::vector<std::shared_ptr<SensorShape>>& sensors*/)
        : m_blueprint(blueprint)//, m_sensors(sensors)
    {
        // Origin on the middle of the rear wheel axle
        m_obb.setSize(sf::Vector2f(m_blueprint.length, m_blueprint.width));
        m_obb.setOrigin(m_blueprint.back_overhang, m_obb.getSize().y / 2);

        // Undefined states
        update(sf::Vector2f(NAN, NAN), NAN);
    }

    void addSensor(std::shared_ptr<SensorShape> shape)
    {
       m_sensors.push_back(shape);
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void update(sf::Vector2f const& position, float const heading)
    {
        m_position = position;
        m_heading = heading;
        m_obb.setPosition(position);
        m_obb.setRotation(RAD2DEG(heading));

        size_t i(BLUEPRINT::WheelName::MAX);
        while (i--)
        {
            m_blueprint.wheels[i].position =
                position + HEADING(m_blueprint.wheels[i].offset, heading);
        }

        for (auto const& it: m_sensors)
        {
            it->update(position, heading);
        }
    }

    //--------------------------------------------------------------------------
    //! \brief const getter: return the oriented bounding box (OBB) of the shape.
    //--------------------------------------------------------------------------
    inline sf::RectangleShape const& obb() const
    {
        return m_obb;
    }

    //--------------------------------------------------------------------------
    //! \brief const getter: return the oriented bounding box (OBB) of the shape.
    //! \param[in] nth the nth wheel.
    //! \param[in] heading the heading of the vehicle in radian.
    //--------------------------------------------------------------------------
    inline sf::RectangleShape obb_wheel(size_t const nth, float steering) const
    {
        assert(nth < m_blueprint.wheels.size());
        auto const& w = m_blueprint.wheels[nth];

        sf::RectangleShape shape(sf::Vector2f(w.radius * 2.0f, w.width));
        shape.setOrigin(shape.getSize().x / 2.0f, shape.getSize().y / 2.0f);
        shape.setPosition(w.position);
        shape.setRotation(RAD2DEG(m_heading + steering));
        return shape;
    }

    //--------------------------------------------------------------------------
    //! \brief Check if the shape collides with another vehicle shape.
    //! \param[out] p: return the minimum translation vector in case of
    //! collision \return true in case of collision and return the position of
    //! the collision.
    //--------------------------------------------------------------------------
    inline bool collides(sf::RectangleShape const& other, sf::Vector2f& p) const
    {
        return ::collide(m_obb, other, p); // FIXME Can be cached ?
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

    //--------------------------------------------------------------------------
    //! \brief Return the vehicle blueprint
    //--------------------------------------------------------------------------
    inline BLUEPRINT const& blueprint() const
    {
        return m_blueprint;
    }

    //--------------------------------------------------------------------------
    //! \brief Return the number of wheels
    //--------------------------------------------------------------------------
    inline size_t countWheels() const
    {
        return m_blueprint.wheels.size();
    }

private:

    //! \brief
    BLUEPRINT& m_blueprint;
    //! \brief Cache information from VehiclePhysics
    sf::Vector2f m_position;
    //! \brief Cache information from VehiclePhysics
    float m_heading;
    //! \brief Sensors shapes
    std::vector<std::shared_ptr<SensorShape>> m_sensors;
    //! \brief Oriented bounding box for attitude and collision
    sf::RectangleShape m_obb;
};

#endif
