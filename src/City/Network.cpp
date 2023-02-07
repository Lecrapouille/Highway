//==============================================================================
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
//==============================================================================

#include "City/Network.hpp"
#include "Math/Math.hpp"

// =============================================================================
// NODE
// =============================================================================

// -----------------------------------------------------------------------------
Node::Node(size_t const id, sf::Vector2<Meter> const& position)
    : m_id(id), m_position(position)
{}

// -----------------------------------------------------------------------------
Way* Node::getWayToNode(Node const& node)
{
    size_t i = m_ways.size();
    while (i--)
    {
        if (((m_ways[i]->m_from == &node) && (m_ways[i]->m_to == this)) ||
            ((m_ways[i]->m_to == &node) && (m_ways[i]->m_from == this)))
        {
            return m_ways[i];
        }
    }

    return nullptr;
}

// =============================================================================
// SEGMENT
// =============================================================================

// -----------------------------------------------------------------------------
Way::Way(size_t const id, Node& node1, Node& node2)
    : m_id(id), m_from(&node1), m_to(&node2)
{
    m_from->m_ways.push_back(this);
    m_to->m_ways.push_back(this);
    updateMagnitude();
}

// -----------------------------------------------------------------------------
void Way::updateMagnitude()
{
    m_magnitude = math::distance(m_to->position(), m_from->position());
}

// =============================================================================
// PATH
// =============================================================================

// -----------------------------------------------------------------------------
Node& Path::addNode(sf::Vector2<Meter> const& position)
{
    m_nodes.push_back(std::make_unique<Node>(m_nextNodeId++, position/*, *this*/));
    return *m_nodes.back();
}

// -----------------------------------------------------------------------------
// TODO: replace existing segment or allow multi-graph (== speedway)
Way& Path::addWay(Node& p1, Node& p2)
{
    m_ways.push_back(std::make_unique<Way>(m_nextWayId++, p1, p2/*, *this*/));
    return *m_ways.back();
}

// -----------------------------------------------------------------------------
Node& Path::splitWay(Way& segment, double const offset)
{
    if (offset <= 0.0)
        return segment.from();
    if (offset >= 1.0)
        return segment.to();

    sf::Vector2<Meter> poff(segment.m_to->position() - segment.m_from->position());
    poff.x *= offset;
    poff.y *= offset;
    sf::Vector2<Meter> wordPosition = segment.m_from->position() + poff;

    Node& newNode = addNode(wordPosition);
    addWay(newNode, segment.to());

    auto& segs = segment.m_to->m_ways;
    segs.erase(std::remove(segs.begin(), segs.end(), &segment));
    segment.m_to = &newNode;
    segment.m_to->m_ways.push_back(&segment);
    segment.updateMagnitude();

    return newNode;
}