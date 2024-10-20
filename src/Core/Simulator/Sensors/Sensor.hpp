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
    Sensor(std::string const& p_type, std::string const& p_name,
           sensor::BluePrint const& p_blueprint, sf::Color const& p_color,
           bool p_enabled)
        : blueprint(p_blueprint), type(p_type), name(p_name), color(p_color),
          enable(p_enabled)
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
        return std::make_unique<SensorShape>(*this, blueprint, color);
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void update(Second const dt)
    {
        if (!enable)
            return ;
        onUpdate(dt);
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual void onUpdate(Second const dt) = 0;

public:

    //! \brief Dimension of the sensor.
    sensor::BluePrint const blueprint;
    //! \brief Sensor type.
    std::string const type;
    //! \brief Sensor name.
    std::string const name;
    //! \brief Current sensor color.
    sf::Color color;
    //! \brief Is the sensor running ?
    bool enable = true;
};

// ****************************************************************************
//! \brief
// ****************************************************************************
class Antenna: public Sensor
{
public:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    Antenna(std::string const& p_name, sensor::BluePrint const& p_blueprint,
            sf::Color const& p_color, bool p_enabled)
        : Sensor("antenna", p_name, p_blueprint, p_color, p_enabled)
    {}

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual void onUpdate(Second const) override
    {
        std::cout << "Antenna Sensor onUpdate: " << name << ": " << enable << std::endl;
    }
};