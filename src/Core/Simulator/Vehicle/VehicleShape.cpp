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

#include "Core/Simulator/Vehicle/ECUs/BodyControlModule.hpp"
#include "Core/Simulator/Vehicle/VehicleShape.hpp"
#include "Core/Simulator/Vehicle/WheelShape.hpp"
#include "Core/Simulator/Vehicle/LightShape.hpp"
#include "Core/Simulator/Vehicle/Vehicle.hpp"
#include "Application/Renderer/Drawable.hpp"
#include "Core/Math/Collide.hpp"
#include "Core/Math/Math.hpp"
#include <array>

//------------------------------------------------------------------------------
VehicleShape::VehicleShape(Vehicle const& vehicle)
    : SceneNode("vehicle"), blueprint(vehicle.blueprint), m_vehicle(vehicle),
      m_wheels_shapes(createChild<SceneNode>("wheels")),
      m_light_shapes(createChild<SceneNode>("lights")),
      m_sensor_shapes(createChild<SceneNode>("sensors"))
{
    // Scene graph: set the origin of the vehicle placed on the middle of
    // its rear wheel axle. FIXME: should be setOrigin(...)
    m_shape.setOrigin(
    {
        float(m_vehicle.blueprint.back_overhang.value()),
        float(m_vehicle.blueprint.width.value()) / 2.0f
    });

    // Oriented bounding box of the vehicle.
    m_shape.setSize(
    {
        float(m_vehicle.blueprint.length.value()),
        float(m_vehicle.blueprint.width.value())
    });
    m_shape.setFillColor(vehicle.color);
    m_shape.setOutlineThickness(OUTLINE_THICKNESS);
    m_shape.setOutlineColor(sf::Color::Black);

    // Create wheel shapes as scene graph from the blueprint.
    auto const wheels = m_vehicle.wheels();
    m_wheels_shapes.createChild<WheelShape>("RR",
        wheels[vehicle::BluePrint::Where::RR],
        m_vehicle.blueprint.wheels[vehicle::BluePrint::Where::RR]);
    m_wheels_shapes.createChild<WheelShape>("RL",
        wheels[vehicle::BluePrint::Where::RL],
        m_vehicle.blueprint.wheels[vehicle::BluePrint::Where::RL]);
    m_wheels_shapes.createChild<WheelShape>("FR",
        wheels[vehicle::BluePrint::Where::FR],
        m_vehicle.blueprint.wheels[vehicle::BluePrint::Where::FR]);
    m_wheels_shapes.createChild<WheelShape>("FL",
        wheels[vehicle::BluePrint::Where::FL],
        m_vehicle.blueprint.wheels[vehicle::BluePrint::Where::FL]);
}

//------------------------------------------------------------------------------
void VehicleShape::onUpdate()
{
    setPosition(float(m_vehicle.position().x.value()),
                float(m_vehicle.position().y.value()));
    setRotation(float(Degree(m_vehicle.heading()).value()));
}

//------------------------------------------------------------------------------
void VehicleShape::onDraw(sf::RenderTarget& target, sf::RenderStates const& states) const
{
    // Draw the car body.
    target.draw(m_shape, states);

    // Draw the position of the car.
    target.draw(Circle(m_vehicle.position(), 0.01_m, sf::Color::Black, 8u));

    // Other shapes are draw automatically when iterating on the scene graph.
}

//------------------------------------------------------------------------------
void VehicleShape::addSensorShape(SensorShape::Ptr shape)
{
    m_sensor_shapes.getOrCreateDummy(shape->owner().type).attachChild(std::move(shape));
}

//------------------------------------------------------------------------------
void VehicleShape::addLightShape(std::string const& type, std::string const& name,
    lights::BluePrint const& bp, sf::Color color, bool const& bulb_state)
{
    m_light_shapes.getOrCreateDummy(type).createChild<LightShape>(name, bp, bulb_state, color);
}