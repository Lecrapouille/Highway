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

#ifndef NETWORK_HPP
#  define NETWORK_HPP

#  include "Math/Math.hpp"
#  include <SFML/Graphics/RectangleShape.hpp>
#  include <deque>
#  include <map>
#  include <memory>

class Way;
class Path;

// =============================================================================
//! \brief Class defining the extremity of arcs constituing a path. This
//! class can be seen as nodes of a graph (named Path). This class is not a
//! basic structure holding position but it holds more information such the list
//! of neighbor Ways (graph arcs) and Units (houses, buildings) refering to it.
//! Units consumn and output Agent carrying Resources along a Path and Node are
//! origin and destination for Agents.
// =============================================================================
class Node
{
    friend Way;
    friend Path;

public:

    using Ptr = std::unique_ptr<Node>;

    // -------------------------------------------------------------------------
    //! \brief Uninitialized internal states. Only friend classes can then
    //! initialize fileds.
    // -------------------------------------------------------------------------
    Node() = default;

    // -------------------------------------------------------------------------
    //! \brief Initialized internal states.
    //! \param[in] id: a unique id used to reference the node.
    //! \param[in] position: the position in the world.
    // -------------------------------------------------------------------------
    Node(size_t const id, sf::Vector2<Meter> const& position);

    // -------------------------------------------------------------------------
    //! \brief Constructor by copy.
    // -------------------------------------------------------------------------
    Node(Node const&) = default;

    // -------------------------------------------------------------------------
    //! \brief Check if the node is not has ways (aka not orphan).
    // -------------------------------------------------------------------------
    bool hasWays() const { return m_ways.size() > 0u; }

    // -------------------------------------------------------------------------
    //! \brief Return the first segment in which the given node belongs to.
    //! \note to get the full list, call ways().
    //! \param[in] node: the neighbor node.
    //! \return the address of the segment where extremity points are node and
    //! this instance. Return nullptr if the node was not a neighbor.
    // -------------------------------------------------------------------------
    Way* getWayToNode(Node const& node);

    // -------------------------------------------------------------------------
    //! \brief Return the unique identifier.
    // -------------------------------------------------------------------------
    size_t id() const { return m_id; }

    // -------------------------------------------------------------------------
    //! \brief Return the position inside the World coordinate.
    // -------------------------------------------------------------------------
    sf::Vector2<Meter> const& position() const { return m_position; }

    // -------------------------------------------------------------------------
    //! \brief Const getter of Ways hold by this instance.
    // -------------------------------------------------------------------------
    std::vector<Way*>& ways() { return m_ways; }

private:

    //! \brief Unique identifier.
    size_t m_id;
    //! \brief World position.
    sf::Vector2<Meter> m_position;
    //! \brief Ways owning this node instance.
    std::vector<Way*> m_ways;
};

// =============================================================================
//! \brief Class defining a segment inside a Path. An Way can been seen as an
//! arc on an undirected graph. An Way is the locomotion for Agents carrying
//! Resources. A way is defined by two Nodes. Arcs have no
//! direction because we (currently) do not manage one-way traffic.
// =============================================================================
class Way
{
    friend Node;
    friend Path;

public:

    using Ptr = std::unique_ptr<Way>;

    Way() = delete;

    // -------------------------------------------------------------------------
    //! \brief Initialized the state of the Way.
    //! \param[in] id: unique identifier.
    //! \param[in] type: const reference of a given type of Way also refered
    //! internally. The refered instance shall not be deleted before this Way
    //! instance is destroyed.
    //! \param[in] from: The node of origin.
    //! \param[in] to: The node of destination.
    // -------------------------------------------------------------------------
    Way(size_t const id, Node& from, Node& to);

    // -------------------------------------------------------------------------
    //! \brief Return the unique identifier.
    // -------------------------------------------------------------------------
    size_t id() const { return m_id; }

    // -------------------------------------------------------------------------
    //! \brief Return the origin node.
    // -------------------------------------------------------------------------
    Node& from() { return *m_from; }

    // -------------------------------------------------------------------------
    //! \brief Return the destination node.
    // -------------------------------------------------------------------------
    Node& to() { return *m_to; }

    // -------------------------------------------------------------------------
    //! \brief Return the length of the segment that has been computed by
    //! updateLength().
    // -------------------------------------------------------------------------
    Meter magnitude() const { return m_magnitude; }

private:

    // -------------------------------------------------------------------------
    //! \brief Compute the length of the segment.
    // -------------------------------------------------------------------------
    void updateMagnitude();

private:

    //! \brief Unique identifier.
    size_t m_id;
    //! \brief Node of origin.
    Node *m_from = nullptr;
    //! \brief Node of destination.
    Node *m_to = nullptr;
    //! \brief Cache the computation of the segment length.
    Meter m_magnitude;
};

// =============================================================================
//! \brief Is a Graph, typically player created, holding nodes (Node) and arcs
//! (Way). Ways, connecting Nodes, make up Path sets in which Agent can carry
//! Resources along from an Unit to another Unit. Example of Paths: Dirt roads,
//! highway, one-way road, power lines, water pipes, flight paths ...
// =============================================================================
class Path
{
private:

    using Nodes = std::deque<Node::Ptr>;
    using Ways = std::deque<Way::Ptr>;

public:

    using Ptr = std::unique_ptr<Path>;

    // -------------------------------------------------------------------------
    //! \brief Create and store a new node given its world position.
    //! \return the newly created node.
    // -------------------------------------------------------------------------
    Node& addNode(sf::Vector2<Meter> const& position);

    //TODO void removeNode(Node& node);

    // -------------------------------------------------------------------------
    //! \brief Create and store a new segment given two existing nodes.
    //! \return the newly created segment.
    // -------------------------------------------------------------------------
    Way& addWay(Node& p1, Node& p2);

    //TODO void RemoveWay(SimWay segment)

    // -------------------------------------------------------------------------
    //! \brief Split a segment into two sub arcs linked by a newly created
    //! node (execept if the offset is set to one of the segment extremity)
    //!
    //! \param segment: the segment to split.
    //! \param offset: [0..1] the normalized length from from where to split
    //! the segment.
    //! \return the newly created position if offset = ]0..1[ or return the
    //! segment vertex if offset is 0 or 1.
    // -------------------------------------------------------------------------
    Node& splitWay(Way& segment, double const offset);

    // -------------------------------------------------------------------------
    //! \brief Return the list of nodes.
    // -------------------------------------------------------------------------
    Nodes const& nodes() const { return m_nodes; }

    // -------------------------------------------------------------------------
    //! \brief Return the list of Ways.
    // -------------------------------------------------------------------------
    Ways const& ways() const { return m_ways; }

private:

    //! \brief Holde nodes. Do not use vector<> to avoid references to be
    //! invalidated.
    Nodes m_nodes;
    //! \brief Hold arcs. Do not use vector<> to avoid references to be
    //! invalidated.
    Ways m_ways;
    //! \brief
    size_t m_nextNodeId = 0u;
    //! \brief
    size_t m_nextWayId = 0u;
};

using Paths = std::map<std::string, Path::Ptr>;

#endif