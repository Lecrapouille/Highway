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

#  include "Sensors/Radar.hpp"
#  include "Utils/Utils.hpp"
#  include "Utils/Collide.hpp"
#  include <cassert>

void SensorShape::set(sf::Vector2f const& p, float const orientation)
{
   obb.setPosition(p);
   obb.setRotation(orientation);
}

void Radar::init(SensorShape& shape, const float range)
{
   m_shape = &shape;
   m_shape->obb.setSize(sf::Vector2f(range, 0.1f));
   m_shape->obb.setOrigin(0.0f, m_shape->obb.getSize().y / 2);
   m_shape->obb.setFillColor(sf::Color(165, 42, 42));
   m_shape->obb.setOutlineThickness(ZOOM);
   m_shape->obb.setOutlineColor(sf::Color::Blue);
}

void Radar::set(sf::Vector2f const& p, float const orientation)
{
   assert(m_shape != nullptr);
   m_shape->set(p, orientation);
}

bool Radar::collides(sf::RectangleShape const& shape, sf::Vector2f& p) const
{
   assert(m_shape != nullptr);
   return ::collide(m_shape->obb, shape, p);
}
