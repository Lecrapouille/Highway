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

#include "Core/Simulator/Sensors/SensorShape.hpp"
#include "Core/Simulator/Sensors/Sensor.hpp"

//------------------------------------------------------------------------------
SensorShape::SensorShape(Sensor const& sensor, sensor::BluePrint const& blueprint,
                         sf::Color const& color)
    : SceneNode(sensor.name), m_sensor(sensor), m_color(color),
      m_shape(sf::Vector2f(float(blueprint.dimension.x.value()),
                           float(blueprint.dimension.y.value())))
{
    m_shape.setOrigin(0.0f, m_shape.getSize().y / 2.0f);
    m_shape.setPosition(float(blueprint.offset.x.value()),
                        float(blueprint.offset.y.value()));
    m_shape.setRotation(float(Degree(blueprint.orientation)));

    m_shape.setFillColor(m_color);
    m_shape.setOutlineThickness(OUTLINE_THICKNESS);
    m_shape.setOutlineColor(sf::Color(165, 42, 42));
}

//------------------------------------------------------------------------------
void SensorShape::onUpdate()
{
    visible = m_sensor.enable;
}

//------------------------------------------------------------------------------
void SensorShape::onDraw(sf::RenderTarget& target, sf::RenderStates const& states) const
{
    target.draw(m_shape, states);
}