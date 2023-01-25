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
Parking::Parking(ParkingBluePrint const& bp, sf::Vector2<Meter> const& position, Radian const heading)
    : blueprint(bp), type(convert(bp.angle)),
      m_shape(sf::Vector2f(float(bp.length.value()), float(bp.width.value()))),
      m_heading(heading)
{
    m_shape.setOrigin(sf::Vector2f(0.0f, float(bp.width.value() / 2.0)));
    m_shape.setRotation(-float(Degree(bp.angle + heading)));
    m_shape.setPosition(float(position.x), float(position.y));
    m_shape.setFillColor(sf::Color::White);
    m_shape.setOutlineThickness(OUTLINE_THICKNESS);
    m_shape.setOutlineColor(sf::Color::Black);
}

//------------------------------------------------------------------------------
Parking::Parking(ParkingBluePrint const& bp, sf::Vector2<Meter> const& position, Radian const heading, Car& car)
    : Parking(bp, position, heading)
{
    m_car = &car;
}

//------------------------------------------------------------------------------
void Parking::bind(Car& car)
{
    //if (car.bound())
    //    throw "Car already bound on parking spot";
    //if (m_car != nullptr)
    //    throw "Car already bound on parking spot";

    // Center the car in the parking slot
    Meter x = car.blueprint.back_overhang + (blueprint.length - car.blueprint.length) / 2.0f;
    sf::Vector2<Meter> const offset(x, 0.0_m);

    car.init(0.0_mps_sq, 0.0_mps, position() + math::heading(offset, blueprint.angle + heading()), heading(), 0.0_rad);

    //car.bind();
    m_car = &car;
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
