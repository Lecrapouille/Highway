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

#include "Simulator/City/Road.hpp"
#include "Vehicle/Car.hpp"
#include <random>

static std::mt19937 rng;

//------------------------------------------------------------------------------
LaneBluePrint::LaneBluePrint(Meter const l, Meter const w, Radian const a)
    : length(l), width(w), angle(a)
{
    std::cout << "Lane l: " << l << " w: " << w <<  " a: " << a << std::endl;
}

//------------------------------------------------------------------------------
Lane::Lane(sf::Vector2<Meter> const& start, sf::Vector2<Meter> const& stop,
           Meter const width, TrafficSide s)
    : blueprint(math::distance(start, stop), width, math::orientation(start, stop)),
      side(s), m_shape(sf::Vector2f(float(blueprint.length.value()),
                                    float(blueprint.width.value())))
{
    m_shape.setOrigin(sf::Vector2f(0.0f, float(blueprint.width.value() / 2.0)));
    m_shape.setRotation(float(Degree(blueprint.angle)));
    m_shape.setPosition(float(start.x), float(start.y));
    std::cout << "P " << start.x << ", " << start.y << std::endl;
    m_shape.setFillColor(sf::Color::Blue);
    m_shape.setOutlineThickness(ZOOM);
    m_shape.setOutlineColor(sf::Color::Black);
}

//------------------------------------------------------------------------------
Road::Road(sf::Vector2<Meter> const& start, sf::Vector2<Meter> const& stop,
           Meter const width, std::array<size_t, TrafficSide::Max> const& lanes)
{
    size_t side = TrafficSide::Max;
    while (side--)
    {
        m_lanes[side].resize(lanes[side]);
    }

    for (size_t i = 0u; i < lanes[TrafficSide::RightHand]; i++)
    {
        m_lanes[TrafficSide::RightHand][i] =
            std::make_unique<Lane>(sf::Vector2<Meter>(start.x + double(i) * width, start.y),
                                   sf::Vector2<Meter>(stop.x + double(i) * width, stop.y),
                                   width, TrafficSide::RightHand);
    }

    for (size_t i = 0u; i < lanes[TrafficSide::LeftHand]; i++)
    {
        m_lanes[TrafficSide::LeftHand][i] =
            std::make_unique<Lane>(sf::Vector2<Meter>(start.x - double(i) * width, start.y),
                                   sf::Vector2<Meter>(stop.x - double(i) * width, stop.y),
                                   width, TrafficSide::LeftHand);
    }
}

#if 0

//------------------------------------------------------------------------------
static sf::Vector2<Meter> initCarPosition(const Lane& lane, float conts xPercent, float const yPercent)
{
    float xPercent = generateRandomPercent(0.0f, 100.f);
    float yPercent = generateRandomPercent(0.0f, 100.0f);

    return {
        lane.x + (lane.blueprint.width * xPercent / 100.0f),
        lane.y + (lane.blueprint.height * yPercent / 100.0f)
    };
}

//------------------------------------------------------------------------------
void Road::bind(Car& car, const Lane& lane, std::Vector2f const& offset)
{
    while (true)
    {
        initCarPosition(car, lane, xPercent, yPercent);
        for (Car const& otherCar: lane.cars())
        {
            if (!car.collides(otherCar))
            {
                m_cars.push_back(car);
                return ;
            }
        }
    }
}

//------------------------------------------------------------------------------
void Road::bind(Car& car, const Lane& lane, std::Vector2f const& offset)
{
    for (const Lane& lane : road.lanes) {
        int numCars = generateRandomPercent(0, MAX_CARS_PER_LANE);
        for (int i = 0; i < numCars; i++) {

#endif