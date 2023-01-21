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

#  include "Sensors/Antenna.hpp"
#  include "Math/Collide.hpp"
#  include "Simulator/City/City.hpp"

//------------------------------------------------------------------------------
Antenna::Antenna(AntennaBluePrint const& blueprint_, const char* name_,
                 City const& city_, sf::Color const& color_)
    : Sensor(blueprint_, name_, color_), blueprint(blueprint_), m_city(city_)
{
    shape.setSize(blueprint.range, 0.1_m);
}

//------------------------------------------------------------------------------
void Antenna::update(Second const dt)
{
    m_detection.valid = false;
    for (auto const& car: m_city.cars()) // TODO m_city.collidables()
    {
        if (detects(car->obb(), m_detection.position))
        {
            m_detection.valid = true;
            m_detection.distance = math::distance(m_detection.position, shape.position());
            std::cout << "******* DETECTION " << m_detection.distance << " P: " << m_detection.position.x << ", " << m_detection.position.x << std::endl;
            return ;
        }
    }
}

//------------------------------------------------------------------------------
bool Antenna::detects(sf::RectangleShape const& other, sf::Vector2<Meter>& position) const
{
    sf::Vector2f p;
    const bool res = ::collide(shape.obb(), other, p);
    position = sf::Vector2<Meter>(Meter(p.x), Meter(p.y));
    return res;
}