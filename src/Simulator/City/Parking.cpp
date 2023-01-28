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

#include "Math/Math.hpp"
#include "City/Parking.hpp"
#include "Vehicle/Car.hpp"

//------------------------------------------------------------------------------
ParkingBluePrint::ParkingBluePrint(Meter const l, Meter const w, Degree const a)
    : length(l), width(w), angle(a)
{}

//------------------------------------------------------------------------------
static Parking::Type convert(Degree const angle)
{
    size_t a = size_t(angle.value());

    switch (a)
    {
    case 0u:
        return Parking::Type::Parallel;
    case 45u:
        return Parking::Type::Diagonal45;
    case 60u:
        return Parking::Type::Diagonal60;
    case 75u:
        return Parking::Type::Diagonal75;
    case 90u:
        return Parking::Type::Perpendicular;
    default:
        assert(false && "Unknown parking type");
        return Parking::Type::Parallel;
    }
}

//------------------------------------------------------------------------------
Parking::Parking(ParkingBluePrint const& bp, sf::Vector2<Meter> const& position,
                 Radian const heading)
    : blueprint(bp), type(convert(bp.angle)),
      m_shape(sf::Vector2f(float(bp.length.value()), float(bp.width.value()))),
      m_heading(heading)
{
    // The origin of the parking is placed on its Top-Left corner.
    // Note that initially the origin of the parking was placed on its
    // Center-Left. Initially, this seemed nice for computing auto-park
    // trajectories because once the car is parked its position (middle of the
    // rear axle) is placed at the origin of the parking slot; but this origin
    // is not nice for placing parkings along roads because we to add the offset
    // "Top-Left" => "Center-Left" in the formula.
    m_shape.setOrigin(sf::Vector2f(0.0f, float(bp.width.value()))); // Top-Left
    // m_shape.setOrigin(sf::Vector2f(0.0f, float(bp.width.value() / 2.0))); // Center-Left
    m_shape.setRotation(-float(Degree(bp.angle + heading)));
    m_shape.setPosition(float(position.x), float(position.y));
    m_shape.setFillColor(sf::Color::White);
    m_shape.setOutlineThickness(OUTLINE_THICKNESS);
    m_shape.setOutlineColor(sf::Color::Black);
}

//------------------------------------------------------------------------------
Parking::Parking(ParkingBluePrint const& bp, sf::Vector2<Meter> const& position,
                 Radian const heading, Car& car)
    : Parking(bp, position, heading)
{
    m_car = &car;
}

#if 0 // TBD Idea
//------------------------------------------------------------------------------
void Parking::setOrigin(Origin const where)
{
    switch (where)
    {
    case Origin::TopLeft:
        m_shape.setOrigin(sf::Vector2f(0.0f, float(bp.width.value())));
        break;
    case Origin::CenterLeft:
        m_shape.setOrigin(sf::Vector2f(0.0f, float(bp.width.value() / 2.0)));
        break;
    default:
        assert(false && "Unknow Origin placement");
        break;
    }
}
#endif

//------------------------------------------------------------------------------
bool Parking::bind(Car& car)
{
    if (!setOccupied(car))
        return false;

    // Parking coordinates: center the car along the parking slot length.
    Meter x = car.blueprint.back_overhang +
              (blueprint.length - car.blueprint.length) / 2.0f;
    sf::Vector2<Meter> const offset(x, -blueprint.width / 2.0);

    // Convert the offset in the world coordinates.
    sf::Vector2<Meter> const p = position() + math::heading(offset, blueprint.angle - m_heading);

    // Init vehicle states.
    car.init(0.0_mps_sq, 0.0_mps, p, blueprint.angle - m_heading, 0.0_rad);

    return true;
}

//------------------------------------------------------------------------------
bool Parking::setOccupied(Car& car)
{
    if (m_car != nullptr)
    {
        if (m_car == &car)
        {
            LOGE("You have already bound");
        }
        else
        {
            LOGE("Car already bound on parking spot");
        }
        return false;
    }

    m_car = &car;
    return true;
}

//------------------------------------------------------------------------------
void Parking::unbind()
{
    if (m_car == nullptr)
        return ;

    //m_car.bind(false);
    m_car = nullptr;
}

//------------------------------------------------------------------------------
bool Parking::empty() const
{
    return m_car == nullptr;
}

//------------------------------------------------------------------------------
Car& Parking::car()
{
    assert(m_car != nullptr);
    return *m_car;
}
