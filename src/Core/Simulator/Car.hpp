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

#  include "Core/Simulator/BluePrints.hpp"
#  include "Core/Simulator/Vehicle/Vehicle.hpp"

#  include <SFML/Graphics.hpp>

#  include <sstream>


class Car: public Vehicle
{
public:

    //------------------------------------------------------------------------------
    Car(const char* p_model, const char* p_name, sf::Color const& p_color)
        : Vehicle(BluePrints::instance().get<vehicle::BluePrint>(p_model), p_name, p_color)
    {}

    virtual std::string isValid() const
    {
        std::stringstream ss;

        if (m_physics == nullptr)
        {
            ss << "The vehicle " << name << " does not have a controller. "
               << "Have you called Car::setPhysicModel() ?";
            return ss.str();
        }
/*
        if (m_control == nullptr)
        {
            ss << "The vehicle " << name << " does not have a controller. "
               << "Have you called Car::setController() ?";
            return ss.str();
        }
*/
        auto const& p = physic();
        if (std::isnan(p.speed().value()) || std::isnan(p.heading().value()) ||
            std::isnan(p.position().x.value()) || std::isnan(p.position().y.value()))
        {
            ss << "The vehicle " << name << " states have not been set. "
               << "Have you called init() ?";
            return ss.str();
        }

        return {}; // success
    }   
};