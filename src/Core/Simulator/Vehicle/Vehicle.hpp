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

#pragma once

#  include "Core/Simulator/Vehicle/PhysicModel.hpp"
#  include <SFML/Graphics/Color.hpp>

#  include <functional>
#  include <map>

// ****************************************************************************
//! \brief
// ****************************************************************************
class Vehicle
{
public:

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    typedef std::function<void()> Callback;

    //-------------------------------------------------------------------------
    //! \brief
    //! \warning After this constructor, your instance will still be partially
    //! initialized. You have to set, in this oerder: a physic model with
    //! setPhysicModel(), a controler with setController() and initialize speed,
    //! acceleration and wih init().
    //-------------------------------------------------------------------------
    Vehicle(vehicle::BluePrint const& p_blueprint, const char* p_name, sf::Color const& p_color)
        : blueprint(p_blueprint), name(p_name), color(p_color), //, initial_color(p_color),
          m_shape(p_blueprint)
    {
        //m_control = std::make_unique<VehicleControl>();
    }

    //-------------------------------------------------------------------------
    //! \brief 
    //-------------------------------------------------------------------------
    template<class Model, typename ...Args>
    void setPhysicModel(Args&&... args)
    {
        m_physics = std::move(vehicle::PhysicModel::create<Model>(
            m_shape, std::forward<Args>(args)...));
    }

    //-------------------------------------------------------------------------
    //! \brief Initialize first value for the physics.
    //! \param[in] position: position of the middle of the rear axle.
    //! \param[in] heading: initial yaw of the car [rad]
    //! \param[in] speed: initial longitudinal speed [m/s] (usually 0).
    //! \param[in] steering: initial front wheels orientation [rad] (usually 0).
    //-------------------------------------------------------------------------
    virtual void init(MeterPerSecondSquared const acceleration, MeterPerSecond const speed,
                      sf::Vector2<Meter> const& position, Radian const heading,
                      Radian const steering = 0.0_deg);

    //-------------------------------------------------------------------------
    // External or internal: collides with city and sensor detecs city ???
    //-------------------------------------------------------------------------
    virtual void update(Second const dt);

    //-------------------------------------------------------------------------
    //! \brief Register a callback for reacting to SFML press events.
    //-------------------------------------------------------------------------
    inline void addCallback(size_t const key, Callback&& cb)
    {
        m_callbacks[key] = cb;
    }

    //-------------------------------------------------------------------------
    //! \brief Call callbacks when an key was pressed (if the key was registered).
    //! \return true if the SFML I/O was known, else return false.
    //-------------------------------------------------------------------------
    bool reactTo(size_t const key);

    //-------------------------------------------------------------------------
    //! \brief Const getter: return the car shape.
    //-------------------------------------------------------------------------
    inline VehicleShape const& shape() const
    {
        return m_shape;
    }

    //-------------------------------------------------------------------------
    //! \brief Const getter: return the oriented bounding box of the vehicle.
    //-------------------------------------------------------------------------
    inline sf::RectangleShape const& obb() const
    {
        return m_shape.obb();
    }

    vehicle::PhysicModel const& physic() const
    {
        assert((m_physics != nullptr) && "You did not calle setPhysicModel()");
        return *m_physics;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return longitudinal acceleration [meter/second^2].
    //--------------------------------------------------------------------------
    inline MeterPerSecondSquared const& acceleration() const
    {
        return m_physics->acceleration();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the longitudinal speed [meter/second].
    //--------------------------------------------------------------------------
    inline MeterPerSecond const& speed() const
    {
        return m_physics->speed();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the position of the middle of the rear axle
    //! inside the world coordinates.
    //--------------------------------------------------------------------------
    inline sf::Vector2<Meter> const& position() const
    {
        return m_physics->position();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading (yaw angle) [rad].
    //--------------------------------------------------------------------------
    inline Radian const& heading() const
    {
        return m_physics->heading();
    }

public:

    //! \brief Dimension of the vehicle
    vehicle::BluePrint const blueprint;
    //! \brief Car's name
    std::string const name;
    //! \brief Current car color. Public: to allow to change it for distinguish
    //! car between them or for showing collisions ...
    sf::Color color;

protected:

    //! \brief The shape of the vehicle, dimension, wheel positions.
    VehicleShape m_shape;
    //! \brief Kinematic, Dynamic model of the vehicle
    std::unique_ptr<vehicle::PhysicModel> m_physics = nullptr;
    //! \brief The cruse control
    //std::unique_ptr<VehicleControl> m_control = nullptr;
    //! \brief List of reactions to do when events occured
    std::map<size_t, Callback> m_callbacks;

    //! \brief Save initial color
    // sf::Color initial_color;
};