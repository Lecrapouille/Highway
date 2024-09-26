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

#  include "Core/Math/Movable.hpp"
#  include "Core/Simulator/Vehicle/VehicleShape.hpp"

namespace vehicle {

class Shape;

// *****************************************************************************
//! \brief Base class for computing the vehicle physic (kinematic, dynamics ...)
//! need to implement void Movable::update(float const dt)
// *****************************************************************************
class PhysicModel : public Movable
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor: aggregation of the vehicle shape and its
    //! control.
    //--------------------------------------------------------------------------
    explicit PhysicModel(VehicleShape const& /*shape*/)
    {}

    //--------------------------------------------------------------------------
    //! \brief Needed because of virtual methods.
    //--------------------------------------------------------------------------
    virtual ~PhysicModel() = default;

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual void init(MeterPerSecondSquared const acceleration,
                      MeterPerSecond const speed, sf::Vector2<Meter> const position,
                      Radian const heading) = 0;

    //--------------------------------------------------------------------------
    //! \brief Update discrete time equations from continuous time equations.
    //--------------------------------------------------------------------------
    virtual void update(Second const dt) = 0;

    //--------------------------------------------------------------------------
    //! \brief Create a new PhysicModel derived instance. 
    //--------------------------------------------------------------------------
    template<class Model, typename ...Args>
    static std::unique_ptr<Model> create(VehicleShape const& shape, Args&&... args)
    {
        //static_assert(std::is_base_of<Model, PhysicModel>::value,
        //              "Try adding an instance not deriving from PhysicModel");
        return std::make_unique<Model>(shape, std::forward<Args>(args)...);
    }
};

} // namespace vehicle