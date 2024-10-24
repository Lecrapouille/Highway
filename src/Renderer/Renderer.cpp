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

#include "Renderer/Renderer.hpp"
#include "City/City.hpp"
#include "ECUs/AutoParkECU/AutoParkECU.hpp" // FIXME to be removed https://github.com/Lecrapouille/Highway/issues/15
#include "Sensors/Radar.hpp" // FIXME temporary

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
    // Draw the origin of the lane
    target.draw(Circle(lane.origin(), 0.01_m, sf::Color::Black, 8u), states);
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

    // Draw the origin of the road
    target.draw(Circle(road.origin(), 0.03_m, sf::Color::Red, 8u), states);

    side = TrafficSide::Max;
    while (side--)
    {
        for (auto const& it: road.m_lanes[side])
        {
            target.draw(Circle(it->origin(), 0.01_m, sf::Color::Black, 8u), states);
        }
    }
}

//------------------------------------------------------------------------------
void Renderer::draw(Parking const& parking, sf::RenderTarget& target, sf::RenderStates const& states)
{
    // Draw the parking shape
    sf::RectangleShape shape = parking.obb();
    shape.setFillColor(parking.color);
    shape.setOutlineThickness(0.1f);
    shape.setOutlineColor(sf::Color(255, 161, 7));

    target.draw(shape, states);
    // Draw the position and the origin of the parking
    target.draw(Circle(parking.position(), 0.01_m, sf::Color::Black, 8u), states);
    target.draw(Circle(parking.origin(), 0.01_m, sf::Color::Red, 8u), states);
}

//------------------------------------------------------------------------------
void Renderer::draw(Car const& car, sf::RenderTarget& target, sf::RenderStates const& states)
{
    // Draw the car body
    sf::RectangleShape body = car.obb();
    body.setFillColor(car.collided() ? COLISION_COLOR : car.color);
    body.setOutlineThickness(OUTLINE_THICKNESS);
    body.setOutlineColor(sf::Color::Blue);
    target.draw(body, states);

    // Draw the position of the car
    target.draw(Circle(car.position(), 0.01_m, sf::Color::Black, 8u));

    // Draw the car wheels
    size_t i = car.wheels().size();
    while (i--)
    {
        sf::RectangleShape wheel = car.obb_wheel(i);
        wheel.setFillColor(sf::Color::Black);
        wheel.setOutlineThickness(OUTLINE_THICKNESS);
        wheel.setOutlineColor(sf::Color::Yellow);
        target.draw(wheel, states);
    }

    // Draw the car sensors
    for (auto const& it: car.sensors())
    {
        if (!it->renderable)
            continue ;
        target.draw(it->shape.obb(), states);

        // FIXME Radar: ugly !!!!!!!
        Radar* r = dynamic_cast<Radar*>(it.get());
        if (r != nullptr)
        {
            target.draw(r->coverageArea(), states); // FIXME buggy the arc does not turn
        }
    }

    // Draw Turning indicators
    bool left_light, right_light;
    car.turningIndicator.getLights(left_light, right_light);
    i = car.blueprint.turning_indicators.size();
    while (i--)
    {
        sf::RectangleShape shape = car.shape().turning_indicator(i);
        const bool r = (right_light) && ((i == CarBluePrint::Where::RR) || (i == CarBluePrint::Where::FR));
        const bool l = (left_light) && ((i == CarBluePrint::Where::RL) || (i == CarBluePrint::Where::FL));
        if (r || l)
        {
            shape.setFillColor(sf::Color::Yellow);
        }
        else
        {
            shape.setFillColor(sf::Color::Black);
        }
        target.draw(shape, states);
    }

    // Draw Lights
    bool light = true;
    i = car.blueprint.lights.size();
    while (i--)
    {
        sf::RectangleShape shape = car.shape().light(i);
        const bool front = light && ((i == CarBluePrint::Where::FL) || (i == CarBluePrint::Where::FR));
        const bool rear = light && ((i == CarBluePrint::Where::RL) || (i == CarBluePrint::Where::RR));
        if (front)
        {
            shape.setFillColor(sf::Color::Yellow);
        }
        else
        {
            shape.setFillColor(sf::Color::Black);
        }
        if (rear) // TODO brake pressed (use listener ?)
        {
            shape.setFillColor(sf::Color::Red);
        }
        else
        {
            shape.setFillColor(sf::Color::Black);
        }
        target.draw(shape, states);
    }

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
