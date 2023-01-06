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

#include "Renderer/Renderer.hpp"
#include "City/City.hpp"
#include "ECUs/AutoParkECU/AutoParkECU.hpp" // FIXME to be removed https://github.com/Lecrapouille/Highway/issues/15

//------------------------------------------------------------------------------
//void Renderer::draw(SpatialHashGrid const& hashgrid, sf::RenderTarget& target, sf::RenderStates const& states)
//{
//    Grid grid(hashgrid.bounds(), hashgrid.dimensions(), sf::Color::Black); // FIXME heavy
//    target.draw(grid, states);
//}

//------------------------------------------------------------------------------
void Renderer::draw(Lane const& lane, sf::RenderTarget& target, sf::RenderStates const& states)
{
    sf::RectangleShape const& shape = lane.shape();
    target.draw(shape, states);
    target.draw(Circle(shape.getPosition().x, shape.getPosition().y,
                       ZOOM, sf::Color::Black), states);
}

//------------------------------------------------------------------------------
void Renderer::draw(Road const& road, sf::RenderTarget& target, sf::RenderStates const& states)
{
    size_t side = TrafficSide::Max;
    while (side--)
    {
        for (auto const& it: road.m_lanes[side])
        {
            draw(*it, target, states);
        }
    }
}

//------------------------------------------------------------------------------
void Renderer::draw(Parking const& parking, sf::RenderTarget& target, sf::RenderStates const& states)
{
    sf::RectangleShape const& shape = parking.obb();
    target.draw(shape, states);
    target.draw(Circle(float(parking.position().x.value()), 
                       float(parking.position().y.value()), 
                       ZOOM, sf::Color::Black), states);
}

//------------------------------------------------------------------------------
void Renderer::draw(Car const& car, sf::RenderTarget& target, sf::RenderStates const& states)
{
    // Car body.
    sf::RectangleShape body = car.obb();
    body.setFillColor(car.collided() ? COLISION_COLOR : car.color);
    body.setOutlineThickness(ZOOM);
    body.setOutlineColor(sf::Color::Blue);
    target.draw(body, states);
    target.draw(Circle(float(car.position().x.value()),
                       float(car.position().y.value()), ZOOM, sf::Color::Black));

    // Car wheels
    size_t i = car.wheels().size();
    while (i--)
    {
        sf::RectangleShape wheel = car.obb_wheel(i);
        wheel.setFillColor(sf::Color::Black);
        wheel.setOutlineThickness(ZOOM);
        wheel.setOutlineColor(sf::Color::Yellow);
        target.draw(wheel, states);
    }

    // Car sensors
    for (auto const& it: car.sensors())
    {
        if (!it->renderable)
            continue ;
        // Radar: target.draw(it->coverageArea(), states);
        target.draw(it->shape.obb(), states);
    }

    // Turning indicator https://github.com/Lecrapouille/Highway/issues/17

    // TODO Trailers https://github.com/Lecrapouille/Highway/issues/16

    // Debug Trajectory https://github.com/Lecrapouille/Highway/issues/15
    // FIXME find better solution. Shall not know AutoParkECU but ECU and maybe ECU::draw
    if (car.isEgo() && car.hasECU<AutoParkECU>())
    {
        AutoParkECU const& ecu = car.getECU<AutoParkECU>();
        if (ecu.hasTrajectory())
        {
            ecu.trajectory().draw(target, states);
        }
    }
}

// TBD https://github.com/Lecrapouille/Highway/issues/18
// Draw City from void Simulator::drawSimulation()
// void Renderer::draw(City const& city, sf::RenderTarget& target, sf::RenderStates const& states)
