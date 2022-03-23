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
        //m_obb.setOrigin(0.0f, m_obb.getSize().y / 2.0f);
        m_obb.setFillColor(sf::Color(165, 42, 42));
        m_obb.setOutlineThickness(ZOOM);

        // Undefined states
        update(sf::Vector2f(NAN, NAN), NAN);
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void update(sf::Vector2f const& position, float const heading)
    {
        m_position = position;
        m_heading = heading;
        m_obb.setPosition(position);
        m_obb.setRotation(RAD2DEG(heading + blueprint.orientation));
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

   sf::Vector2f m_position;
   float m_heading;
   //! \brief Oriented bounding box
   sf::RectangleShape m_obb;
};

#endif
