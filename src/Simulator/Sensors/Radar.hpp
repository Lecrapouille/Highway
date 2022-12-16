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

#ifndef CAR_SENSORS_RADAR_HPP
#  define CAR_SENSORS_RADAR_HPP

#  include "Simulator/Actor.hpp"
#  include "Simulator/Sensors/SensorShape.hpp"
#  include "Renderer/Drawable.hpp"
#  include <cassert>

// FIXME class Sensor: Observer { detect() => notify } utiliser la lib gtk + forward declaration

class Car; // FIXME: publi Sensor

// TODO https://www.mathworks.com/help/driving/ref/drivingradardatagenerator-system-object.html

struct RadarBluePrint: public DynamicActor, public SensorBluePrint
{
   // FIXME: why needed ?
   RadarBluePrint(sf::Vector2f off, float ori, float f, float r)
     : SensorBluePrint(off, ori), fov(f), range(r)
   {}

   //! \brief Field Of View: Angular field of view of radar [deg].
   float const fov;
   //! \brief Mmaximum range of radar [meter].
   float const range;
};

// ****************************************************************************
//! \brief Very ultra basic sensor detecting parked cars. This sensor is certainly
//! not realist for works as a bug entenna and detect oriented boundind boxes
//! collisions.
// ****************************************************************************
class Radar: public SensorShape
{
public:

   //--------------------------------------------------------------------------
   //! \brief Default constructor: bind a bluiprint and set the sensor rangle.
   //! \param[in] range [m].
   //! \fixme should be in constructor but since a vehicle has a sensors but its
   //! shape holds SensorShape we had to split it. I dunno how to fix that.
   //--------------------------------------------------------------------------
   Radar(RadarBluePrint const& blueprint);//SensorBluePrint const& blueprint, float const fov, float const range);

   //--------------------------------------------------------------------------
   //! \brief Is the sensor collides to the given bounding box ?
   //! \param[in] shape: the bounding box (of a parked car ie).
   //! \param[inout] p the point of collision.
   //! \return true if the sensor has detected a box.
   //--------------------------------------------------------------------------
   bool detects(sf::RectangleShape const& shape, sf::Vector2f& p);// FIXME  const;

   Arc const& coverageArea()
   {
      m_coverage_area.init(m_position.x, m_position.y, blueprint.range,
                           blueprint.orientation + m_heading - blueprint.fov,
                           blueprint.orientation + m_heading + blueprint.fov,
                           sf::Color::Red);
      return m_coverage_area;
    }

public:

   //! \brief
   RadarBluePrint const blueprint;

private:

   //! \brief Shape of the radar coverage area.
   Arc m_coverage_area;
};

#endif
