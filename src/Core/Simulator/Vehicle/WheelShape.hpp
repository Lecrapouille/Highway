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
#  include "Core/Simulator/Vehicle/Wheel.hpp"
#  include "Core/Simulator/Vehicle/BluePrint.hpp"

// *****************************************************************************
//! \brief
// *****************************************************************************
class WheelShape: public SceneNode
{
public:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    WheelShape(std::string const& name, Wheel const& wheel, wheel::BluePrint const& blueprint);

private: // Inheritance from SceneNode

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual void onUpdate();

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:

    Wheel const& m_wheel;
    sf::RectangleShape m_shape;
};