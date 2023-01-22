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

#include "Simulator/City/Road.hpp"
#include "Vehicle/Car.hpp"
#include <random>
#include <algorithm>

static std::mt19937 rng;

//------------------------------------------------------------------------------
LaneBluePrint::LaneBluePrint(Meter const l, Meter const w, Radian const a)
    : length(l), width(w), angle(a)
{
    // std::cout << "Lane l: " << l << " w: " << w <<  " a: " << a << std::endl;
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
    std::cout << "Lane " << (s == TrafficSide::RightHand ? "right " : "left ") << start.x << ", " << start.y << std::endl;
    if (s == TrafficSide::RightHand)
        m_shape.setFillColor(COLOR_DRIVING_LANE);
    else
        m_shape.setFillColor(COLOR_RESTRICTED_LANE);
    m_shape.setOutlineThickness(0.1f);
    m_shape.setOutlineColor(sf::Color(255, 161, 7));
}

//------------------------------------------------------------------------------
Road::Road(std::vector<sf::Vector2<Meter>> const& centers,
           Meter const width, std::array<size_t, TrafficSide::Max> const& lanes)
    : m_start(centers[0]), m_stop(centers[1]), m_width(width)
{
    std::cout << "Road P " << m_start.x << ", " << m_start.y << " W " << width << std::endl;
    size_t side = TrafficSide::Max;
    while (side--)
    {
        std::cout << "  Side " << side << ": " << lanes[side] << " lane" << std::endl;
        m_lanes[side].resize(lanes[side]); // FIXME manage 0 size
    }

    for (size_t i = 0u; i < lanes[TrafficSide::RightHand]; i++)
    {
        m_lanes[TrafficSide::RightHand][i] =
            std::make_unique<Lane>(sf::Vector2<Meter>(m_start.x, m_start.y - double(i) * width - 0.5 * width),
                                   sf::Vector2<Meter>(m_stop.x, m_stop.y - double(i) * width - 0.5 * width),
                                   width, TrafficSide::RightHand);
    }

    for (size_t i = 0u; i < lanes[TrafficSide::LeftHand]; i++)
    {
        m_lanes[TrafficSide::LeftHand][i] =
            std::make_unique<Lane>(sf::Vector2<Meter>(m_start.x, m_start.y + double(i) * width + 0.5 * width),
                                   sf::Vector2<Meter>(m_stop.x, m_stop.y + double(i) * width + 0.5 * width),
                                   width, TrafficSide::LeftHand);
    }
}

//------------------------------------------------------------------------------
sf::Vector2<Meter> Road::offset(TrafficSide const side, size_t const desired_lane,
                                double const x, double const y)
{
    assert((x >= 0.0) && (x <= 1.0) && "x shall be a percent");
    assert((y >= 0.0) && (y <= 1.0) && "y shall be a percent");

    Meter const offset_x = math::lerp(m_start.x, m_stop.x, x);
    Meter const offset_y = math::lerp(0.0_m, m_width, y);
    size_t const lane = math::constrain(desired_lane, size_t(0), m_lanes[side].size());

    if (side == TrafficSide::RightHand)
    {
        return { offset_x, m_start.y - offset_y - double(lane) * m_width };
    }
    else
    {
        return { offset_x, m_start.y + offset_y + double(lane) * m_width };
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