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

#include "Core/Simulator/Vehicle/WheelShape.hpp"

//------------------------------------------------------------------------------
WheelShape::WheelShape(std::string const& name, wheel::BluePrint const& blueprint) // &car::steering
    : SceneNode(name),
      m_shape(sf::Vector2f(float(blueprint.radius.value() * 2.0),
                           float(blueprint.thickness.value())))
{
    m_shape.setOrigin(m_shape.getSize().x / 2.0f, m_shape.getSize().y / 2.0f);
    m_shape.setPosition(float(blueprint.offset.x.value()), float(blueprint.offset.y.value()));

    m_shape.setFillColor(sf::Color::Black);
    m_shape.setOutlineThickness(OUTLINE_THICKNESS);
    m_shape.setOutlineColor(sf::Color::Yellow);
}

//------------------------------------------------------------------------------
void WheelShape::onUpdate()
{
    //m_shape.setRotation(float(Degree(m_heading + steering)));
}

//------------------------------------------------------------------------------
void WheelShape::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_shape, states);
}