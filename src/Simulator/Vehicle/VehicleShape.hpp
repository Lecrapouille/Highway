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
#  include "Simulator/Sensors/SensorShape.hpp"
#  include <vector>
#  include <cassert>
#  include <memory>

// *****************************************************************************
//! \brief A vehicle shape is an oriented bounding box of the body (rectangle knowing its
//! position and orientation (here in a 2D world: the heading aka yaw). The shape
//! is used for the rendering process and the collision detection.
//! \tparam BLUEPRINT struct holding dimension of the shape (ie CarBluePrint,
//! TrailerBluePrint ... see VehicleBlueprint.hpp).
// *****************************************************************************
template<class BLUEPRINT>
class VehicleShape
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor with the vehicle dimension (ie CarBluePrint,
    //! TrailerBluePrint ... see Vehiclem_blueprint.hpp).
    //--------------------------------------------------------------------------
    VehicleShape(BLUEPRINT& blueprint)
        : m_blueprint(blueprint)
    {
        // Origin on the middle of the rear wheel axle
        m_obb.setSize(sf::Vector2f(m_blueprint.length, m_blueprint.width));
        m_obb.setOrigin(m_blueprint.back_overhang, m_obb.getSize().y / 2);

        // Undefined states
        update(sf::Vector2f(NAN, NAN), NAN);
    }

    //--------------------------------------------------------------------------
    //! \brief Add the shape of the sensor: when the shape is rotationg, sensor
    //! orientation will also be updated (mimic a scene graph hierarchy).
    //--------------------------------------------------------------------------
    void addSensorShape(std::shared_ptr<SensorShape> shape)
    {
       m_sensors.push_back(shape);
    }

    //--------------------------------------------------------------------------
    //! \brief Refresh the position and heading of shape and child shapes (ie
    //! sensor shapes).
    //--------------------------------------------------------------------------
    void update(sf::Vector2f const& position, float const heading)
    {
        // Update body shape
        m_obb.setPosition(position);
        m_obb.setRotation(RAD2DEG(heading));
        m_heading = heading;

        // Update wheel shape
        size_t i(BLUEPRINT::WheelName::MAX);
        while (i--)
        {
            m_blueprint.wheels[i].position =
                position + HEADING(m_blueprint.wheels[i].offset, heading);
        }

        // Update sensor shape
        for (auto const& it: m_sensors)
        {
            it->update(position, heading);
        }
    }

    //--------------------------------------------------------------------------
    //! \brief Return the vehicle blueprint
    //--------------------------------------------------------------------------
    inline BLUEPRINT const& blueprint() const
    {
        return m_blueprint;
    }

    //--------------------------------------------------------------------------
    //! \brief const getter: return the oriented bounding box (OBB) of the body.
    //--------------------------------------------------------------------------
    inline sf::RectangleShape const& obb() const
    {
        return m_obb;
    }

    //--------------------------------------------------------------------------
    //! \brief const getter: return the oriented bounding box (OBB) of the nth
    //! wheel.
    //! \param[in] nth the nth wheel: shall be < BLUEPRINT::WheelName::MAX.
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
        return ::collide(m_obb, other, p);
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the position of the middle of the rear axle
    //! inside the world coordinates [meter].
    //--------------------------------------------------------------------------
    inline sf::Vector2f position() const
    {
        return  m_obb.getPosition();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading (yaw angle) [rad].
    //--------------------------------------------------------------------------
    inline float heading() const
    {
        return m_heading;
    }

    //--------------------------------------------------------------------------
    //! \brief Return the number of wheels
    //--------------------------------------------------------------------------
    inline size_t countWheels() const
    {
        return BLUEPRINT::WheelName::MAX;
    }

private:

    //! \brief Dimension of the vehicle
    BLUEPRINT m_blueprint;
    //! \brief Sensors shapes
    std::vector<std::shared_ptr<SensorShape>> m_sensors;
    //! \brief Oriented bounding box for attitude and collision
    sf::RectangleShape m_obb;
    //! \brief Cache m_obb::getOrientation() in radian: avoid to convert from degree
    //! since SFML uses radians.
    float m_heading;
};

#endif
