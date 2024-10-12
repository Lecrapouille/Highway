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

#include "Core/Simulator/Vehicle/VehicleShape.hpp"
#include "Core/Simulator/Vehicle/WheelShape.hpp"
#include "Core/Simulator/Vehicle/Vehicle.hpp"
#include "Application/Renderer/Drawable.hpp"
#include "Core/Math/Collide.hpp"
#include "Core/Math/Math.hpp"
#include <array>

//------------------------------------------------------------------------------
VehicleShape::VehicleShape(Vehicle const& vehicle)
    : SceneNode("shape"), blueprint(vehicle.blueprint), m_vehicle(vehicle),
      m_wheels_shapes(createChild<SceneNode>("wheels")),
      m_turning_indicator_shapes(createChild<SceneNode>("turning indicators")),
      m_light_shapes(createChild<SceneNode>("lights"))
{
    static const std::array<std::string, vehicle::BluePrint::Where::MAX> s_names = {
        "RR", "RL", "FL", "FR"
    };

    // Origin on the middle of the rear wheel axle
    m_obb.setSize(sf::Vector2f(float(m_vehicle.blueprint.length.value()),
                               float(m_vehicle.blueprint.width.value())));
    m_obb.setOrigin(sf::Vector2f(float(m_vehicle.blueprint.back_overhang.value()),
                                 m_obb.getSize().y / 2.0f));

    // Create wheel shapes as scene graph from the blueprint
    size_t i = m_vehicle.blueprint.wheels.size();
    while (i--)
    {
        m_wheels_shapes.createChild<WheelShape>(
            s_names.at(i), m_vehicle.wheels()[i], m_vehicle.blueprint.wheels[i]);
    }

#if 0
    // Create turning indicator shapes as scene graph from the blueprint
    auto& turning_indicator_node = createChild<RectShape>("turning indicators");
    for (auto const& it: p_blueprint.turning_indicators)
    {
        turning_indicator_node.createChild<>(""); // FR FL ...
    }

    // Create light shapes as scene graph from the blueprint
    auto& lights = createChild<RectShape>("lights");
    for (auto const& it: p_blueprint.lights)
    {
        lights.createChild<>(""); // FR FL ...
    }
#endif
}

//------------------------------------------------------------------------------
void VehicleShape::onUpdate()
{
    m_obb.setPosition(float(m_vehicle.position().x.value()),
                      float(m_vehicle.position().y.value()));
    m_obb.setRotation(float(Degree(m_vehicle.heading()).value()));
}

//------------------------------------------------------------------------------
void VehicleShape::onDraw(sf::RenderTarget& target, sf::RenderStates const& states) const
{
    // Draw the car body
    sf::RectangleShape body = obb(); // By copy !
    body.setFillColor(/*collided() ? COLLISION_COLOR :*/ sf::Color::Green/*color*/);
    body.setOutlineThickness(OUTLINE_THICKNESS);
    body.setOutlineColor(sf::Color::Blue);
    target.draw(body, states);

    // Draw the position of the car
    target.draw(Circle(m_vehicle.position(), 0.01_m, sf::Color::Black, 8u));

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