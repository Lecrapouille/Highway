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

#include "Application/Renderer/Renderer.hpp"
#include "Application/Renderer/Drawable.hpp"
#include "Core/Simulator/Vehicle/Car.hpp"

#define OUTLINE_THICKNESS 0.01f

//------------------------------------------------------------------------------
void draw(Car const& car, sf::RenderTarget& target, sf::RenderStates const& states)
{
    // Draw the car body
    sf::RectangleShape body = car.obb();
    body.setFillColor(/*car.collided() ? COLISION_COLOR :*/ car.color);
    body.setOutlineThickness(OUTLINE_THICKNESS);
    body.setOutlineColor(sf::Color::Blue);
    target.draw(body, states);

    // Draw the position of the car
    target.draw(Circle(car.position(), 0.01_m, sf::Color::Black, 8u));

#if 0
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
        std::cout << ecu.info << std::endl;
        if (ecu.hasTrajectory())
        {
            ecu.trajectory().draw(target, states);
        }
    }
#endif
}