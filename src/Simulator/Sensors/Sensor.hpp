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
// along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#ifndef CAR_SENSOR_HPP
#  define CAR_SENSOR_HPP

#  include <SFML/Graphics/RectangleShape.hpp>
#  include "Renderer/Drawable.hpp"
#  include "Math/Math.hpp"
#  include "Math/Units.hpp"

// ****************************************************************************
//! \brief
// ****************************************************************************
struct SensorBluePrint
{
    SensorBluePrint(sf::Vector2<Meter> const offset_, Degree const orientation_)
        : offset(offset_), orientation(orientation_)
    {}

    //! \brief Relative position from the car shape position (middle rear axle)
    sf::Vector2<Meter> offset;
    //! \brief Relative relative orientation
    Degree orientation;
};

// ****************************************************************************
//! \brief A sensor shape is just a blueprint used inside of the vehicle shape
//! for orienting automatically the sensor when the vehicle shape is turned.
// ****************************************************************************
class SensorShape
{
public:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    SensorShape(SensorBluePrint const& blueprint_, sf::Color const& color_)
        : blueprint(blueprint_), color(color_)
    {
        // Origin is the middle of the rear wheel axle of the vehicle
        m_obb.setOrigin(0.0f, m_obb.getSize().y / 2.0f);
        m_obb.setFillColor(color);
        m_obb.setOutlineColor(sf::Color(165, 42, 42));
        m_obb.setOutlineThickness(ZOOM);

        // Undefined states
        update(sf::Vector2<Meter>(Meter(NAN), Meter(NAN)), Radian(NAN));
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void update(sf::Vector2<Meter> const& position, Radian const heading)
    {
        const Degree a = heading + blueprint.orientation;
        const sf::Vector2<Meter> p = position + HEADING(blueprint.offset, heading);

        m_obb.setRotation(float(a.value()));
        m_obb.setPosition(float(p.x.value()), float(p.y.value()));
        m_obb.setFillColor(color);
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    inline void setSize(Meter const& x, Meter const& y)
    {
        m_obb.setSize(sf::Vector2f(float(x.value()), float(y.value())));
    }

    //--------------------------------------------------------------------------
    //! \brief const getter: return the oriented bounding box (OBB) of the shape.
    //--------------------------------------------------------------------------
    inline sf::RectangleShape const& obb() const
    {
        return m_obb;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the position of the middle of the rear axle
    //! inside the world coordinates [meter].
    //--------------------------------------------------------------------------
    inline sf::Vector2<Meter> position() const
    {
        const auto p = m_obb.getPosition();
        return sf::Vector2<Meter>(Meter(p.x), Meter(p.y));
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading (yaw angle) [rad].
    //--------------------------------------------------------------------------
    inline Radian heading() const
    {
        return Degree(m_obb.getRotation());
    }

public:

    //! \brief
    SensorBluePrint const& blueprint;
    //! \brief Current car color. Public: to allow to change it for distinguish
    //! car between them or for showing collisions ...
    sf::Color color;

protected:

    //! \brief Oriented bounding box
    sf::RectangleShape m_obb;
};

// ****************************************************************************
//! \brief
// ****************************************************************************
class Sensor
{
public:

    Sensor(SensorBluePrint const& blueprint_, const char* name_, sf::Color const& color_)
        : name(name_), shape(blueprint_, color_)
    {}
    virtual ~Sensor() = default;

    // FIXME shall memorize all detected points from any Collidable object.
    virtual bool detects(sf::RectangleShape const& other, sf::Vector2f& p) = 0;// FIXME  const;

public:

    //! \brief Car's name
    std::string name;
    //! \brief The shape of the sensor
    SensorShape shape;
    //! \brief Renderer or not the sensor
    bool renderable = true;
};

#endif
