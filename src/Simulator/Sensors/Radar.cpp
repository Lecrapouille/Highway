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

#  include "Sensors/Radar.hpp"
#  include "Math/Collide.hpp"
#  include <cassert>

//-----------------------------------------------------------------------------
Radar::Radar(RadarBluePrint const& blueprint_)//SensorBluePrint const& blueprint_, float const fov_, float const range_)
   : SensorShape(blueprint_), blueprint(blueprint_)//, fov(fov_), range(range_),
     //m_coverage_area(0.0f, 0.0f, blueprint.range, blueprint.orientation - blueprint.fov, blueprint.orientation + blueprint.fov)
{
   m_obb.setSize(sf::Vector2f(float(blueprint.range.value()), 0.1f));
   m_obb.setOrigin(0.0f, m_obb.getSize().y / 2.0f);
   m_obb.setOutlineColor(sf::Color::Blue);
}

//-----------------------------------------------------------------------------
bool Radar::detects(sf::RectangleShape const& shape, sf::Vector2f& p) //const
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
