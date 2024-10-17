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

#  include "Core/Simulator/Sensors/SensorShape.hpp"

// ****************************************************************************
//! \brief
// ****************************************************************************
class Sensor
{
public:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    Sensor(sensor::BluePrint const& p_blueprint, std::string const& p_name,
           sf::Color const& p_color)
        : blueprint(p_blueprint), name(p_name), color(p_color)
    {}

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual ~Sensor() = default;

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    SensorShape::Ptr shape()
    {
        return std::make_unique<SensorShape>(name, blueprint, color);
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual void update(Second const dt) = 0;

public:

    //! \brief Dimension of the sensor
    sensor::BluePrint const blueprint;
    //! \brief Sensor name/type
    std::string const name;
    //! \brief Current sensor color.
    sf::Color color;
};

// ****************************************************************************
//! \brief
// ****************************************************************************
class Antenna: public Sensor
{
public:
    using Sensor::Sensor;

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual void update(Second const) override {}
};