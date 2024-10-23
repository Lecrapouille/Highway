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

#pragma once

#  include "Core/Common/SceneGraph.hpp"
#  include "Core/Simulator/Vehicle/BluePrint.hpp"

// *****************************************************************************
//! \brief
// *****************************************************************************
class LightShape: public SceneNode
{
public:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    LightShape(std::string const& name, lights::BluePrint const& blueprint,
               bool const& enabled, sf::Color const& color)
        : SceneNode(name), m_enable(enabled), m_color(color),
          m_shape(sf::Vector2f(float(blueprint.dimension.x.value()),
                               float(blueprint.dimension.y.value())))
    {
        setOrigin(m_shape.getSize().x / 2.0f, m_shape.getSize().y / 2.0f);
        setPosition(float(blueprint.offset.x.value()), float(blueprint.offset.y.value()));

        m_shape.setFillColor(m_color);
        m_shape.setOutlineThickness(OUTLINE_THICKNESS);
        m_shape.setOutlineColor(sf::Color(165, 42, 42));
    }

private: // Inheritance from SceneNode

    //--------------------------------------------------------------------------
    //! \brief Update the visibility of the shape to the renderer.
    //--------------------------------------------------------------------------
    virtual void onUpdate() override
    {
        //bool left_light, right_light;
    //m_turning_indicator.getLights(left_light, right_light); // TODO getLightStates
        visible = m_enable;
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual void onDraw(sf::RenderTarget& target, sf::RenderStates const& states) const override
    {
        target.draw(m_shape, states);
    }

private:

    bool const& m_enable;
    sf::Color m_color;
    sf::RectangleShape m_shape;
};