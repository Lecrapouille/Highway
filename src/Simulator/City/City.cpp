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

#include "City/City.hpp"
#include <iostream>

//------------------------------------------------------------------------------
City::City()
        // FIXME https://github.com/Lecrapouille/Highway/issues/23
        // : m_grid(sf::Rect<float>(0.01f, 0.01f, 1024.0f, 1024.0f), sf::Vector2u(16u, 16u))
{}

//------------------------------------------------------------------------------
void City::reset()
{
    m_car_id = m_ego_id = m_ghost_id = 0u;

    m_ghosts.clear();
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
Road& City::addRoad(std::vector<sf::Vector2<Meter>> const& centers,
                    Meter const width, std::array<size_t, TrafficSide::Max> lanes)
{
    m_roads.push_back(std::make_unique<Road>(centers, width, lanes));
    return *m_roads.back();
}

//------------------------------------------------------------------------------
Parking& City::addParking(const char* type, sf::Vector2<Meter> const& position)
{
    m_parkings.push_back(std::make_unique<Parking>(BluePrints::get<ParkingBluePrint>(type), position));
    return *m_parkings.back();
}

//------------------------------------------------------------------------------
Car& City::addEgo(const char* model, sf::Vector2<Meter> const& position, Degree const heading,
                  MeterPerSecond const speed)
{
    if (m_ego != nullptr)
    {
        LOGW("Ego car already created. Old will be replaced!");
    }

    std::string name = "ego" + std::to_string(m_ego_id++);
    m_ego = createCar<Car>(model, name.c_str(), EGO_CAR_COLOR, 0.0_mps_sq, speed,
                           position, heading, 0.0_rad);
    return *m_ego;
}

//------------------------------------------------------------------------------
Car& City::addEgo(const char* model, Road const& road, TrafficSide const side,
                  size_t const lane, double const offset_long, double const offset_lat,
                  MeterPerSecond const speed)
{
    return addEgo(model, road.offset(side, lane, offset_long, offset_lat), road.heading(side), speed);
}

//------------------------------------------------------------------------------
Car& City::addCar(const char* model, sf::Vector2<Meter> const& position, Degree const heading,
                  MeterPerSecond const speed, Degree const steering)
{
    std::string name = "car" + std::to_string(m_car_id++);
    m_cars.push_back(createCar<Car>(model, name.c_str(), CAR_COLOR, 0.0_mps_sq,
                                    speed, position, heading, steering));
    return *m_cars.back();
}

//------------------------------------------------------------------------------
Car& City::addCar(const char* model, Road const& road, TrafficSide const side,
                  size_t const lane, double const offset_long, double const offset_lat,
                  MeterPerSecond const speed)
{
    return addCar(model, road.offset(side, lane, offset_long, offset_lat), road.heading(side), speed);
}

//------------------------------------------------------------------------------
Car& City::addCar(const char* model, Parking& parking)
{
    Car& car = addCar(model, sf::Vector2<Meter>(0.0_m, 0.0_m), 0.0_deg, 0.0_mps, 0.0_deg);
    parking.bind(car);
    return car;
}

//------------------------------------------------------------------------------
Car& City::addGhost(const char* model, sf::Vector2<Meter> const& position, Degree const heading,
                    Degree const steering)
{
    std::string name = "ghost" + std::to_string(m_ghost_id++);
    m_ghosts.push_back(createCar<Car>(model, name.c_str(), sf::Color::White,
                                      0.0_mps_sq, 0.0_mps, position, heading, steering));
    return *m_ghosts.back();
}
