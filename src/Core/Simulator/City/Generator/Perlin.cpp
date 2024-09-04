//==============================================================================
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
//==============================================================================

#include "Core/Simulator/City/Generator/Perlin.hpp"

void perlin(sf::Image& image, sf::Vector2u const& dimension, std::function<sf::Color(const double, const double)> lambda)
{
    unsigned int const width = dimension.x;
    unsigned int const height = dimension.y;
    image.create(width, height);

    for (unsigned int y = 0u; y < height; ++y)
    {
        for (unsigned int x = 0u; x < width; ++x)
        {
            sf::Color brightness = lambda(double(x), double(y));
            image.setPixel(x, y, brightness);
        }
    }
}
