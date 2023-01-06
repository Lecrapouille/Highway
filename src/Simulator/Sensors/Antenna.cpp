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

#  include "Sensors/Antenna.hpp"
#  include "Math/Collide.hpp"

//------------------------------------------------------------------------------
Antenna::Antenna(AntennaBluePrint const& blueprint_, const char* name_,
                 sf::Color const& color_)
    : Sensor(blueprint_, name_, color_), blueprint(blueprint_)
{
    shape.setSize(blueprint.range, 0.1_m);
}

//------------------------------------------------------------------------------
bool Antenna::detects(sf::RectangleShape const& other, sf::Vector2f& p)
{
    bool res = ::collide(shape.obb(), other, p);
    if (res)
    {
        shape.color = sf::Color::Red;
    }
    else
    {
        shape.color = sf::Color::Green;
    }
    return res;
}