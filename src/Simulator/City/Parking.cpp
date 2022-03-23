// 2021 -- 2022 Quentin Quadrat quentin.quadrat@gmail.com
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

#include "Math/Math.hpp"
#include "City/Parking.hpp"
#include "Vehicle/Car.hpp"

//------------------------------------------------------------------------------
//template<> std::unordered_map<std::string, std::map<std::string, ParkingBluePrint>> BluePrints::m_databases<ParkingBluePrint>;

//------------------------------------------------------------------------------
ParkingBluePrint::ParkingBluePrint(float const l, float const w, size_t const a)
    : length(l), width(w), angle(DEG2RAD(float(a))), deg(a)
{}

//------------------------------------------------------------------------------
static Parking::Type convert(size_t angle) // [deg]
{
    switch (angle)
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
Parking::Parking(ParkingBluePrint const& bp, sf::Vector2f const& position)
    : blueprint(bp), type(convert(bp.deg)), m_shape(sf::Vector2f(bp.length, bp.width))
{
    m_shape.setOrigin(sf::Vector2f(0.0f, bp.width / 2.0f));
    m_shape.setRotation(RAD2DEG(bp.angle));
    m_shape.setPosition(position);
    m_shape.setFillColor(sf::Color::White);
    m_shape.setOutlineThickness(ZOOM);
    m_shape.setOutlineColor(sf::Color::Black);
}

//------------------------------------------------------------------------------
Parking::Parking(ParkingBluePrint const& d, sf::Vector2f const& p, Car& car)
    : Parking(d, p)
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

    float x = car.blueprint.back_overhang + (blueprint.length - car.blueprint.length) / 2.0f;
    sf::Vector2f const offset(x, 0.0f);
    car.init(position() + HEADING(offset, blueprint.angle), heading(), 0.0f, 0.0f);

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
