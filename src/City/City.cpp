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

//------------------------------------------------------------------------------
City::City()
        // FIXME https://github.com/Lecrapouille/Highway/issues/23
        // : m_grid(sf::Rect<float>(0.01f, 0.01f, 1024.0f, 1024.0f), sf::Vector2u(16u, 16u))
{}

//------------------------------------------------------------------------------
void City::reset()
{
    LOGI("Reset city");

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
    LOGI("Add road: start (%g m, %g m), stop (%g m, %g m), width %g m",
         centers[0].x, centers[0].y, centers[1].x, centers[1].y, width);

    m_roads.push_back(std::make_unique<Road>(centers, width, lanes));
    return *m_roads.back();
}

//------------------------------------------------------------------------------
Parking& City::addParking(const char* type, sf::Vector2<Meter> const& position,
                          Radian const heading)
{
    LOGI("Add parking: positon (%g m, %g m), heading %g deg", position.x,
         position.y, Degree(heading));

    m_parkings.push_back(std::make_unique<Parking>(
                             BluePrints::get<ParkingBluePrint>(type),
                             position, heading));
    return *m_parkings.back();
}

//------------------------------------------------------------------------------
Parking& City::addParking(const char* type, Road const& road, TrafficSide const side,
                          double const offset_long, double const offset_lat)
{
    return addParking(type, road.offset(side, 0u, offset_long, offset_lat),
                      -road.heading(side));
}

//------------------------------------------------------------------------------
Parking& City::addParking(Parking const& parking)
        //, double const offset_long, double const offset_lat)
{
    char type[8];
    snprintf(type, 8, "epi.%zu", size_t(parking.blueprint.angle.value()));
    return addParking(type, parking.delta(), parking.heading());
}

//------------------------------------------------------------------------------
Car& City::addEgo(const char* model, sf::Vector2<Meter> const& position,
                  Radian const heading, MeterPerSecond const speed)
{
    char name[8];
    snprintf(name, 8, "ego%zu", m_ego_id++);

    LOGI("Add Ego car '%s': position (%g m, %g m), heading %g deg, speed %g mps",
         name, position.x, position.y, Degree(heading), speed);

    if (m_ego != nullptr)
    {
        LOGW("Ego car already created. Old will be replaced!");
    }

    m_ego = createCar<Car>(model, name, EGO_CAR_COLOR, 0.0_mps_sq, speed,
                           position, heading, 0.0_rad);
    return *m_ego;
}

//------------------------------------------------------------------------------
Car& City::addEgo(const char* model, Road const& road, TrafficSide const side,
                  size_t const lane, double const offset_long,
                  double const offset_lat, MeterPerSecond const speed)
{
    return addEgo(model, road.offset(side, lane, offset_long, offset_lat),
                  road.heading(side), speed);
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

    m_cars.push_back(createCar<Car>(model, name, CAR_COLOR, 0.0_mps_sq, speed,
                                    position, heading, steering));
    return *m_cars.back();
}

//------------------------------------------------------------------------------
Car& City::addCar(const char* model, Road const& road, TrafficSide const side,
                  size_t const lane, double const offset_long,
                  double const offset_lat, MeterPerSecond const speed)
{
    return addCar(model, road.offset(side, lane, offset_long, offset_lat),
                  road.heading(side), speed);
}

//------------------------------------------------------------------------------
Car& City::addCar(const char* model, Parking& parking)
{
    Car& car = addCar(model, sf::Vector2<Meter>(0.0_m, 0.0_m), 0.0_deg,
                      0.0_mps, 0.0_deg);
    parking.bind(car);
    return car;
}

//------------------------------------------------------------------------------
Car& City::addGhost(const char* model, sf::Vector2<Meter> const& position,
                    Radian const heading, Radian const steering)
{
    char name[16];
    snprintf(name, 16, "ghost%zu", m_ghost_id++);

    LOGI("Add ghost car '%s': position (%g m, %g m), heading %g deg",
         name, position.x, position.y, Degree(heading));

    m_ghosts.push_back(createCar<Car>(model, name, sf::Color::White, 0.0_mps_sq,
                                      0.0_mps, position, heading, steering));
    return *m_ghosts.back();
}
