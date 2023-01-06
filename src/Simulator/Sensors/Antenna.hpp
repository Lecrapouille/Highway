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

#ifndef CAR_SENSORS_ANTENNA_HPP
#  define CAR_SENSORS_ANTENNA_HPP

#  include "Sensors/Sensor.hpp"

// ****************************************************************************
//! \brief A sensor shape is just a blue print used inside of the vehicle shape
//! for orienting automatically the sensor when the vehicle shape is turned.
// ****************************************************************************
struct AntennaBluePrint: public SensorBluePrint
{
    //--------------------------------------------------------------------------
    //! \brief Set the sensor attitude (position and heading orientation)
    //--------------------------------------------------------------------------
    AntennaBluePrint(sf::Vector2<Meter> const offset_, Degree const orientation_,
                     Meter const range_)
        : SensorBluePrint(offset_, orientation_), range(range_)
    {}

    //! \brief Mmaximum range of radar [meter].
    Meter const range;
};

// ****************************************************************************
//! \brief
// ****************************************************************************
class Antenna : public Sensor
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor: bind a bluiprint and set the sensor rangle.
    //! \param[in] range [m].
    //! \fixme should be in constructor but since a vehicle has a sensors but its
    //! shape holds AntennaShape we had to split it. I dunno how to fix that.
    //--------------------------------------------------------------------------
    Antenna(AntennaBluePrint const& blueprint_, const char* name_, sf::Color const& color_);

    //--------------------------------------------------------------------------
    //! \brief Is the sensor collides to the given bounding box ?
    //! \param[in] shape: the bounding box (of a parked car ie).
    //! \param[inout] p the point of collision.
    //! \return true if the sensor has detected a box.
    //--------------------------------------------------------------------------
    virtual bool detects(sf::RectangleShape const& other, sf::Vector2f& p) override; // FIXME  const;

public:

    AntennaBluePrint const& blueprint;
};

#endif
