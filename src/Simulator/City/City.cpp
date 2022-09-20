//=====================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2022 Quentin Quadrat <lecrapouille@gmail.com>
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
// along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#include "City/City.hpp"
#include <iostream>

//------------------------------------------------------------------------------
City::City()
    //: m_grid(sf::Rect<float>(0.01f, 0.01f, 1024.0f, 1024.0f), sf::Vector2u(16u, 16u))
{}

//------------------------------------------------------------------------------
void City::reset()
{
    m_car_id = m_ego_id = m_ghost_id = 0u;

    //m_ghosts.clear();
    m_cars.clear();
    m_parkings.clear();
}

//------------------------------------------------------------------------------
Car* City::get(const char* name)
{
   for (auto& it: m_cars)
   {
      if (it->name == name)
         return &(*it);
   }

   return nullptr;
}

//------------------------------------------------------------------------------
Parking& City::addParking(const char* type, sf::Vector2f const& position)
{
    m_parkings.push_back(std::make_unique<Parking>(BluePrints::get<ParkingBluePrint>(type), position));
    return *m_parkings.back();
}

//------------------------------------------------------------------------------
Car& City::addEgo(const char* model, sf::Vector2f const& position, float const heading, float const speed)
{
    std::string name = "ego" + std::to_string(m_ego_id++);

    return createCar<Car>(model, name.c_str(), EGO_CAR_COLOR, 0.0f, speed,
        position, heading, 0.0f);
}

//------------------------------------------------------------------------------
Car& City::addCar(const char* model, sf::Vector2f const& position, float const heading,
                  float const speed, float const steering)
{
    std::string name = "car" + std::to_string(m_car_id++);

    return createCar<Car>(model, name.c_str(), CAR_COLOR, 0.0f, speed, position,
       heading, steering);
}

//------------------------------------------------------------------------------
Car& City::addCar(const char* model, Parking& parking)
{
    Car& car = addCar(model, sf::Vector2f(0.0f, 0.0f), 0.0f, 0.0f);
    parking.bind(car);
    return car;
}

//------------------------------------------------------------------------------
Car& City::addGhost(const char* model, sf::Vector2f const& position, float const heading,
                    float const steering)
{
    std::string name = "ghost" + std::to_string(m_ghost_id++);

    return createCar<Car>(model, name.c_str(), sf::Color::White, 0.0f, 0.0f, position,
       heading, steering);
}
