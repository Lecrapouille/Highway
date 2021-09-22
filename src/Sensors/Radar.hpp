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

class Car;

struct SensorShape
{
   void set(sf::Vector2f const& p, float const orientation);

   //! \brief Relative position from the car shape position (middle rear axle)
   sf::Vector2f offset;
   //! \brief Relative relative orientation
   float orientation;
   //! \brief Oriented bounding box
   sf::RectangleShape obb;
};

class Radar
{
public:

   void init(SensorShape& shape, const float range);
   void set(sf::Vector2f const& p, float const orientation);
   bool collides(sf::RectangleShape const& shape, sf::Vector2f& p) const;

private:

   SensorShape* m_shape = nullptr;
};

#endif