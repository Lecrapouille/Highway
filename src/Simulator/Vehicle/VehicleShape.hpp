//=====================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2023 Quentin Quadrat <lecrapouille@gmail.com>
//
// This file is part of Highway.
//
// Highway is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Highway.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#ifndef VEHICLESHAPE_HPP
#  define VEHICLESHAPE_HPP

#  include "Math/Collide.hpp"
#  include "Math/Math.hpp"
#  include "Simulator/Sensors/Sensor.hpp"
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
class VehicleShape // FIXME better to use scene graph
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
        const sf::Vector2f s(float(m_blueprint.length.value()),
                             float(m_blueprint.width.value()));
        m_obb.setSize(s);
        const sf::Vector2f o(float(m_blueprint.back_overhang.value()),
                             m_obb.getSize().y / 2.0f);
        m_obb.setOrigin(o);

        // Undefined states
        update(sf::Vector2<Meter>(Meter(NAN), Meter(NAN)), Radian(NAN));
    }

    //--------------------------------------------------------------------------
    //! \brief Add the shape of the sensor: when the shape is rotationg, sensor
    //! orientation will also be updated (mimic a scene graph hierarchy).
    //--------------------------------------------------------------------------
    void addSensorShape(SensorShape& shape) // FIXME SensorShape const& ?
    {
       m_sensor_shapes.push_back(&shape);
    }

    //--------------------------------------------------------------------------
    //! \brief Refresh the position and heading of shape and child shapes (ie
    //! sensor shapes).
    //--------------------------------------------------------------------------
    void update(sf::Vector2<Meter> const& position, Radian const heading)
    {
        // Update body shape
        m_obb.setPosition(float(position.x.value()), float(position.y.value()));
        m_obb.setRotation(float(Degree(heading).value()));
        m_heading = heading;

        // Update wheel shape
        size_t i(BLUEPRINT::WheelName::MAX);
        while (i--)
        {
            m_blueprint.wheels[i].position =
                position + math::heading(m_blueprint.wheels[i].offset, heading);
        }

        // Update sensor shape
        for (auto const& it: m_sensor_shapes)
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
    inline sf::RectangleShape obb_wheel(size_t const nth, Radian steering) const
    {
        assert(nth < m_blueprint.wheels.size());
        auto const& w = m_blueprint.wheels[nth];

        sf::Vector2f s(float(w.radius.value() * 2.0), float(w.width.value()));
        sf::RectangleShape shape(s);
        shape.setOrigin(shape.getSize().x / 2.0f, shape.getSize().y / 2.0f);
        shape.setPosition(float(w.position.x.value()), float(w.position.y.value()));
        shape.setRotation(float(Degree(m_heading + steering)));
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
    inline sf::Vector2<Meter> position() const
    {
        const auto p = m_obb.getPosition();
        return sf::Vector2<Meter>(p.x, p.y);
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading (yaw angle) [rad].
    //--------------------------------------------------------------------------
    inline Radian heading() const
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

    //--------------------------------------------------------------------------
    //! \brief Return shapes of sensors
    //--------------------------------------------------------------------------
    inline std::vector<SensorShape*> const& sensorShapes() const
    {
        return m_sensor_shapes;
    }

private:

    //! \brief Dimension of the vehicle
    BLUEPRINT m_blueprint;
    //! \brief Sensors shapes
    std::vector<SensorShape*> m_sensor_shapes;
    //! \brief Oriented bounding box for attitude and collision
    sf::RectangleShape m_obb;
    //! \brief Cache m_obb::getOrientation() in radian: avoid to convert from degree
    //! since SFML uses radians.
    Radian m_heading;
};

#endif
