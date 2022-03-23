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

#ifndef CAR_SENSORS_RADAR_HPP
#  define CAR_SENSORS_RADAR_HPP

#  include "Sensors/SensorShape.hpp"
#  include "Renderer/Drawable.hpp"
#  include <cassert>

class Car;

// TODO https://www.mathworks.com/help/driving/ref/drivingradardatagenerator-system-object.html

struct RadarBluePrint: public SensorBluePrint
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
   bool detects(sf::RectangleShape const& shape, sf::Vector2f& p);// const;

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
