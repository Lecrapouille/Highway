// 2021 Quentin Quadrat lecrapouille@gmail.com
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <https://unlicense.org>

#include "Parking.hpp"
#include "Car.hpp"

static Parking::Type convert(size_t angle) // [deg]
{
    switch (angle)
    {
    case 0u:
        return Parking::Type::Parallel;
    case 90u:
        return Parking::Type::Perpendicular;
    default:
        return Parking::Type::Parallel;
    }
}

Parking::Parking(ParkingDimension const& d, sf::Vector2f const& p)
    : dim(d), type(convert(d.deg)), m_position(p)
{}

Parking::Parking(ParkingDimension const& d, sf::Vector2f const& p, Car& car)
    : Parking(d, p)
{
    m_car = &car;
}

void Parking::bind(Car& car)
{
    //if (car.bound())
    //    throw "Car already bound on parking spot";
    //if (m_car != nullptr)
    //    throw "Car already bound on parking spot";

    sf::Vector2f const offset(car.dim.back_overhang + car.dim.front_overhang, 0.0f);
    car.init(m_position + ROTATE(offset, dim.angle), 0.0f, dim.angle, 0.0f);
    //car.bind();
    m_car = &car;
}

void Parking::unbind()
{
    if (m_car == nullptr)
        return ;

    //m_car.bind(false);
    m_car = nullptr;
}

bool Parking::empty() const
{
    return m_car == nullptr;
}

Car& Parking::car()
{
    assert(m_car != nullptr);
    return *m_car;
}
