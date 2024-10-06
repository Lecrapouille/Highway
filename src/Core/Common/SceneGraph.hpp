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

// *****************************************************************************
//! \brief
// *****************************************************************************
class SceneNode: public sf::Transformable, public sf::Drawable, private NonCopyable
{
public:

    using Ptr = std::unique_ptr<SceneNode>;

public:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    explicit SceneNode(std::string const& name)
        : m_name(name)
    {}

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    explicit SceneNode(const char* name)
        : m_name(name)
    {}

    //--------------------------------------------------------------------------
    //! \brief Needed because of virtual methods.
    //--------------------------------------------------------------------------
    ~SceneNode() = default;

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void attachChild(SceneNode::Ptr child);

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    template<typename T, typename... Args>
    T& createChild(Args&&... args)
    {
        static_assert(std::is_base_of<SceneNode, T>::value,
            "T shall inherit from SceneNode");

        auto child = std::make_unique<T>(std::forward<Args>(args)...);
        child->m_parent = this;

        T& ref = *child;
        m_children.emplace_back(std::move(child));
        return ref;
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    SceneNode* findChild(std::string const& name);

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
	void update() const;

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
	sf::Vector2f getWorldPosition() const;

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
	sf::Transform getWorldTransform() const;

private: // Inheritance from sf::Drawable

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
	virtual void onUpdate() const
    {
        // Do nothing by default
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
	virtual void onDraw(sf::RenderTarget&, sf::RenderStates const&) const
    {
        // Do nothing by default
    }

private:

    std::string m_name;
    SceneNode* m_parent = nullptr;
    std::vector<SceneNode::Ptr> m_children;
    sf::Transform m_transform;
};