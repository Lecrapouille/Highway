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

#  include "Core/Common/NonCopyable.hpp"
#  include <SFML/Graphics.hpp>
#  include <memory>
#  include <string>

// FIXME
#  define OUTLINE_THICKNESS 0.01f

// *****************************************************************************
//! \brief Hierarchy of objects in which children are transformed relative to
//! their parent. All you have to do is pass the combined transform from parent
//! to children when you draw them, all the way until you reach the final
//! drawable entities (sprites, text, shapes, vertex arrays or your own drawables).
// *****************************************************************************
class SceneNode: public sf::Transformable, public sf::Drawable, private NonCopyable
{
public:

    using Ptr = std::unique_ptr<SceneNode>;

public:

    //--------------------------------------------------------------------------
    //! \brief Dummy constructor. Just set the name of the node.
    //--------------------------------------------------------------------------
    explicit SceneNode(std::string const& name)
        : m_name(name)
    {}

    //--------------------------------------------------------------------------
    //! \brief Dummy constructor. Just set the name of the node.
    //--------------------------------------------------------------------------
    explicit SceneNode(const char* name)
        : m_name(name)
    {}

    //--------------------------------------------------------------------------
    //! \brief Needed because of virtual methods.
    //--------------------------------------------------------------------------
    ~SceneNode() = default;

    //--------------------------------------------------------------------------
    //! \brief Add a new node from an already created node.
    //! \note You have to use std::move().
    //--------------------------------------------------------------------------
    void attachChild(SceneNode::Ptr child);

    //--------------------------------------------------------------------------
    //! \brief Create a new node with its name and optional arguments for its
    //! constructor.
    //--------------------------------------------------------------------------
    template<typename T, typename... Args>
    T& createChild(std::string const& name, Args&&... args)
    {
        static_assert(std::is_base_of<SceneNode, T>::value,
            "T shall inherit from SceneNode");

        auto child = std::make_unique<T>(name, std::forward<Args>(args)...);
        child->m_parent = this;

        T& ref = *child;
        m_children.emplace_back(std::move(child));
        return ref;
    }

    //--------------------------------------------------------------------------
    //! \brief Search recursively a by its name.
    //--------------------------------------------------------------------------
    SceneNode* findChild(std::string const& name);

    //--------------------------------------------------------------------------
    //! \brief Recursively update all node. Each node has its method onUpdate()
    //! called.
    //--------------------------------------------------------------------------
    void update();

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    sf::Vector2f getWorldPosition() const;

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    sf::Transform getWorldTransform() const;

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    std::string const& name() const { return m_name; }

private: // Inheritance from sf::Drawable

    //--------------------------------------------------------------------------
    //! \brief Recursively draw all nodes.
    //--------------------------------------------------------------------------
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:

    //--------------------------------------------------------------------------
    //! \brief Called by update(). By default do nothing. Inherit to update your
    //! entity in this method.
    //--------------------------------------------------------------------------
    virtual void onUpdate()
    {
        // Do nothing by default
    }

    //--------------------------------------------------------------------------
    //! \brief Called by draw(). By default draw nothing. Inherit to draw your
    //! entity in this method.
    //--------------------------------------------------------------------------
    virtual void onDraw(sf::RenderTarget&, sf::RenderStates const&) const
    {
        // Do nothing by default
    }

public:

    //! \brief Skip the drawing of the node (but keep drawing its children).
    bool visible = true;

private:

    std::string m_name;
    SceneNode* m_parent = nullptr;
    std::vector<SceneNode::Ptr> m_children;
    sf::Transform m_transform;
};