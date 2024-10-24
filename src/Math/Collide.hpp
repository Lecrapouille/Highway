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
// along with Highway.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#ifndef COLLIDE_HPP
#  define COLLIDE_HPP

#  include <SFML/Graphics/RectangleShape.hpp>

namespace math {

// Separating Axis Theorem (SAT) collision test
// Minimum Translation Vector (MTV) is returned for the first Oriented Bounding Box (OBB)
bool collide(const sf::RectangleShape& obb1, const sf::RectangleShape& obb2,
             sf::Vector2f& mtv);

} // namespace math

#endif
