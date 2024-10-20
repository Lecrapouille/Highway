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

#include "Core/Common/SceneGraph.hpp"
#include <iostream>

//------------------------------------------------------------------------------
void SceneNode::attachChild(SceneNode::Ptr child)
{
    child->m_parent = this;
    m_children.push_back(std::move(child));
}

//------------------------------------------------------------------------------
SceneNode* SceneNode::findChild(std::string const& name)
{
    if (name == m_name)
    {
        return this;
    }

    for (auto const& child: m_children)
    {
        SceneNode* found = child->findChild(name);
        if (found != nullptr)
        {
            return found;
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------
void SceneNode::update()
{
    onUpdate();
    for (auto const& child: m_children)
    {
        child->update();
    }
}

//------------------------------------------------------------------------------
void SceneNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Apply transform of current node
    states.transform *= getTransform();

    // Draw node and children with changed transform
    if (visible)
    {
        onDraw(target, states);
    }
    for (auto const& child: m_children)
    {
        child->draw(target, states);
    }
}

//------------------------------------------------------------------------------
sf::Vector2f SceneNode::getWorldPosition() const
{
    return getWorldTransform() * sf::Vector2f();
}

//------------------------------------------------------------------------------
sf::Transform SceneNode::getWorldTransform() const
{
    sf::Transform transform = sf::Transform::Identity;

    for (const SceneNode* node = this; node != nullptr; node = node->m_parent)
    {
        transform = node->getTransform() * transform;
    }

    return transform;
}