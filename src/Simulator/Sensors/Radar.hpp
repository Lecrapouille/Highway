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

#ifndef CAR_SENSORS_RADAR_HPP
#  define CAR_SENSORS_RADAR_HPP

#  include "Sensors/Sensor.hpp"

// ****************************************************************************
//! \brief
// ****************************************************************************
struct RadarBluePrint: public SensorBluePrint
{
    RadarBluePrint(sf::Vector2<Meter> const offset_, Degree const orientation_,
                   Degree const fov_, Meter const range_)
        : SensorBluePrint(offset_, orientation_), fov(fov_), range(range_)
    {}

    //! \brief Field Of View: Angular field of view of radar [deg].
    Degree const fov;
    //! \brief Maximum range of radar [meter].
    Meter const range;
};

// ****************************************************************************
//! \brief
// ****************************************************************************
class Radar : public Sensor
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor: bind a bluiprint and set the sensor rangle.
    //! \param[in] range [m].
    //! \fixme should be in constructor but since a vehicle has a sensors but its
    //! shape holds SensorShape we had to split it. I dunno how to fix that.
    //--------------------------------------------------------------------------
    Radar(RadarBluePrint const& blueprint_, const char* name_, sf::Color const& color_);

    //--------------------------------------------------------------------------
    //! \brief Is the sensor collides to the given bounding box ?
    //! \param[in] shape: the bounding box (of a parked car ie).
    //! \param[inout] p the point of collision.
    //! \return true if the sensor has detected a box.
    //--------------------------------------------------------------------------
    virtual bool detects(sf::RectangleShape const& other, sf::Vector2f& p) override; // FIXME  const;

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    Arc const& coverageArea();

public:

    RadarBluePrint const& blueprint;

private:

    //! \brief Shape of the radar coverage area.
    Arc m_coverage_area;
};

#endif
