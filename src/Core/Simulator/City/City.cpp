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

#include "Core/Simulator/City/City.hpp"
#include "MyLogger/Logger.hpp"
#include <stdio.h>

#include "Core/Simulator/Vehicle/PhysicModels/TricycleKinematic.hpp"

#  define COLISION_COLOR sf::Color(255, 0, 0)
#  define DEFAULT_CAR_COLOR sf::Color(25, 130, 118)
#  define DEFAULT_EGO_COLOR sf::Color(124, 99, 197)

//------------------------------------------------------------------------------
City::City()
        // FIXME https://github.com/Lecrapouille/Highway/issues/23
        // : m_grid(sf::Rect<float>(0.01f, 0.01f, 1024.0f, 1024.0f), sf::Vector2u(16u, 16u))
{
    std::cout << "City " << this << std::endl;
}

//------------------------------------------------------------------------------
void City::reset()
{
    LOGI("Reset city");

    m_car_id = m_ego_id = m_ghost_id = 0u;

    m_ghosts.clear();
    m_cars.clear();
    //m_parkings.clear();
}

//------------------------------------------------------------------------------
Car& City::addCar(const char* model, sf::Vector2<Meter> const& position,
                  Radian const heading, MeterPerSecond const speed,
                  Radian const steering)
{
    char name[8];
    snprintf(name, 8, "car%zu", m_car_id++);

    LOGI("Add car '%s': position (%g m, %g m), heading %g deg, speed %g mps",
         name, position.x, position.y, Degree(heading), speed);

    auto car = createCar<vehicle::TricycleKinematic>(model, name, DEFAULT_CAR_COLOR,
        0.0_mps_sq, speed, position, heading, steering);

    std::string error = car->isValid();
    if (!error.empty())
    {
        throw error;
    }

    m_cars.push_back(std::move(car));
    return *m_cars.back();
}

//------------------------------------------------------------------------------
Car& City::addEgo(const char* model, sf::Vector2<Meter> const& position,
                  Radian const heading, MeterPerSecond const speed,
                  Radian const steering)
{
    char name[8];
    snprintf(name, 8, "ego%zu", m_ego_id++);

    LOGI("Add Ego car '%s': position (%g m, %g m), heading %g deg, speed %g mps",
         name, position.x, position.y, Degree(heading), speed);

    if (m_ego != nullptr)
    {
        LOGW("Ego car already created. Old will be replaced!");
    }
    auto car = createCar<vehicle::TricycleKinematic>(model, name, DEFAULT_CAR_COLOR,
        0.0_mps_sq, speed, position, heading, steering);

    std::string error = car->isValid();
    if (!error.empty())
    {
        throw error;
    }

    m_ego = std::move(car);
    return *m_ego;
}