//=====================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2022 Quentin Quadrat <lecrapouille@gmail.com>
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

#ifndef SENSOR_SHAPE_HPP
#  define SENSOR_SHAPE_HPP

#  include "Sensors/SensorBluePrint.hpp"
#  include "Math/Math.hpp"

// ****************************************************************************
//! \brief A sensor shape is just a blue print used inside of the vehicle shape
//! for orienting automatically the sensor when the vehicle shape is turned.
// ****************************************************************************
class SensorShape
{
public:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    SensorShape(SensorBluePrint const& blueprint_)
        : blueprint(blueprint_)
    {
        // Origin on the middle of the rear wheel axle
        //m_obb.setSize(sf::Vector2f(range, 0.1f));
        m_obb.setOrigin(0.0f, m_obb.getSize().y / 2.0f);
        m_obb.setFillColor(sf::Color(165, 42, 42));
        m_obb.setOutlineThickness(ZOOM);

        // Undefined states
        update(sf::Vector2<Meter>(Meter(NAN), Meter(NAN)), Radian(NAN));
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void update(sf::Vector2<Meter> const& position, Radian const heading)
    {
        m_position = position;
        m_heading = heading;

        const Degree a = heading + blueprint.orientation;
        const sf::Vector2<Meter> p = position + HEADING(blueprint.offset, heading);

        m_obb.setRotation(float(a.value()));
        m_obb.setPosition(float(p.x.value()), float(p.y.value()));
    }

    //--------------------------------------------------------------------------
    //! \brief const getter: return the oriented bounding box (OBB) of the shape.
    //--------------------------------------------------------------------------
    inline sf::RectangleShape const& obb() const
    {
        return m_obb;
    }

public:

   //! \brief
   SensorBluePrint const& blueprint;

protected:

   sf::Vector2<Meter> m_position;
   Radian m_heading;
   //! \brief Oriented bounding box
   sf::RectangleShape m_obb;
};

#endif
