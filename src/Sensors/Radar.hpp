// 2021 Quentin Quadrat quentin.quadrat@gmail.com
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

#  include <SFML/Graphics/RectangleShape.hpp>
#  include <SFML/System/Vector2.hpp>
#  include <cassert>

class Car;

// ****************************************************************************
//! \brief A sensor shape is just a blue print used inside of the vehicle shape
//! for orienting automatically the sensor when the vehicle shape is turned.
// ****************************************************************************
struct SensorShape
{
   //--------------------------------------------------------------------------
   //! \brief Set the sensor attitude (position and heading orientation)
   //--------------------------------------------------------------------------
   void set(sf::Vector2f const& position, float const orientation);

   //! \brief Relative position from the car shape position (middle rear axle)
   sf::Vector2f offset;
   //! \brief Relative relative orientation
   float orientation;
   //! \brief Oriented bounding box
   sf::RectangleShape obb;
};

// ****************************************************************************
//! \brief Very ultra basic sensor detecting parked cars. This sensor is certainly
//! not realist for works as a bug entenna and detect oriented boundind boxes
//! collisions.
// ****************************************************************************
class Radar
{
public:

   //--------------------------------------------------------------------------
   //! \brief Default constructor: bind a bluiprint and set the sensor rangle.
   //! \param[in] range [m].
   //! \fixme should be in constructor but since a vehicle has a sensors but its
   //! shape holds SensorShape we had to split it. I dunno how to fix that.
   //--------------------------------------------------------------------------
   void init(SensorShape& shape, const float range);

   //--------------------------------------------------------------------------
   //! \brief Set the sensor attitude (position and heading orientation)
   //--------------------------------------------------------------------------
   void set(sf::Vector2f const& position, float const orientation);

   //--------------------------------------------------------------------------
   //! \brief Is the sensor collides to the given bounding box ?
   //! \param[in] shape: the bounding box (of a parked car ie).
   //! \param[inout] p the point of collision.
   //! \return true if the sensor has detected a box.
   //--------------------------------------------------------------------------
   bool detects(sf::RectangleShape const& shape, sf::Vector2f& p) const;

   inline SensorShape const& shape() const
   {
      assert(m_shape != nullptr);
      return *m_shape;
   }

private:

   // FIXME ideally SensorShape& if possible to replace init() by the constructor!
   SensorShape* m_shape = nullptr;
};

#endif
