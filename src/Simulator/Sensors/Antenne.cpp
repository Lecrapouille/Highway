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

#  include "Sensors/Antenne.hpp"
#  include "Math/Collide.hpp"
#  include <cassert>

//-----------------------------------------------------------------------------
Antenne::Antenne(AntenneBluePrint const& blueprint_)
   : SensorShape(blueprint_), blueprint(blueprint_)
{
   //m_shape = &shape;
   m_obb.setSize(sf::Vector2f(blueprint.range, 0.1f));
   m_obb.setOrigin(0.0f, m_obb.getSize().y / 2.0f);
   //m_obb.setFillColor(sf::Color(165, 42, 42));
   //m_obb.setOutlineThickness(ZOOM);
   m_obb.setOutlineColor(sf::Color::Blue);

   // FIXME to be cleaned
   static int id = 0;
   m_id = id++;
}

//-----------------------------------------------------------------------------
bool Antenne::detects(sf::RectangleShape const& shape, sf::Vector2f& p) // FIXME const
{
   bool res = ::collide(m_obb, shape, p);
   if (res)
   {
      m_obb.setFillColor(sf::Color::Red);
   }
   else
   {
      m_obb.setFillColor(sf::Color::Green);
   }
   return res;
}
