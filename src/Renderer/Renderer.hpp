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

#ifndef RENDERER_HPP
#  define RENDERER_HPP

#  include "Renderer/Drawable.hpp"

class Parking;
class Lane;
class Road;
class Car;
class Autonomous;
//class SpatialHashGrid;

// *****************************************************************************
//! \brief
// *****************************************************************************
class Renderer
{
public:

    static void draw(Parking const& parking, sf::RenderTarget& target, sf::RenderStates const& states = sf::RenderStates::Default);
    static void draw(Lane const& lane, sf::RenderTarget& target, sf::RenderStates const& states = sf::RenderStates::Default);
    static void draw(Road const& road, sf::RenderTarget& target, sf::RenderStates const& states = sf::RenderStates::Default);
    static void draw(Car const& Car, sf::RenderTarget& target, sf::RenderStates const& states = sf::RenderStates::Default);
    static void draw(Autonomous const& Car, sf::RenderTarget& target, sf::RenderStates const& states = sf::RenderStates::Default);
    //static void draw(SpatialHashGrid const& grid, sf::RenderTarget& target, sf::RenderStates const& states = sf::RenderStates::Default);
};

#endif // RENDERER_HPP
