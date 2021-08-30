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

#include "Renderer.hpp"
#include "Utils.hpp"
#include "Car.hpp"
#include "Parking.hpp"

Circle::Circle(float x, float y, float r, sf::Color color)
    : m_shape(r, 100)
{
    m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());
    m_shape.setPosition(x, y);
    m_shape.setFillColor(sf::Color(255, 255, 255, 0));
    m_shape.setOutlineThickness(ZOOM * 2.0f);
    m_shape.setOutlineColor(color);
}

Arc::Arc(float x, float y, float r, float start, float end, sf::Color color)
    : m_shape(r, start, end, 100)
{
    m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());
    m_shape.setPosition(x, y);
    m_shape.setFillColor(sf::Color(255, 255, 255, 0));
    m_shape.setOutlineThickness(ZOOM * 2.0f);
    m_shape.setOutlineColor(color);
}

Arrow::Arrow(const float xa, const float ya, const float xb, const float yb, sf::Color color)
{
    // Arc magnitude
    const float arrowLength = NORM(xa, ya, xb, yb);

    // Orientation
    const float teta = (yb - ya) / (xb - xa);
    float arrowAngle = std::atan(teta) * 180.0f / 3.1415f; // rad -> deg
    if (xb < xa)
        arrowAngle += 180.f;
    else if (yb < ya)
        arrowAngle += 360.f;

    // Head of the arrow
    const sf::Vector2f arrowHeadSize{ ZOOM * 9.0f, ZOOM * 9.0f };
    m_head = sf::ConvexShape{ 3 };
    m_head.setPoint(0, { 0.f, 0.f });
    m_head.setPoint(1, { arrowHeadSize.x, arrowHeadSize.y / 2.f });
    m_head.setPoint(2, { 0.f, arrowHeadSize.y });
    m_head.setOrigin(arrowHeadSize.x, arrowHeadSize.y / 2.f);
    m_head.setPosition(sf::Vector2f(xb, yb));
    m_head.setRotation(arrowAngle);
    m_head.setOutlineThickness(ZOOM);
    m_head.setOutlineColor(color);
    m_head.setFillColor(color);

    // Tail of the arrow.
    const sf::Vector2f tailSize{ arrowLength - arrowHeadSize.x, ZOOM * 1.0f };
    m_tail = sf::RectangleShape{ tailSize };
    m_tail.setOrigin(0.0f, tailSize.y / 2.f);
    m_tail.setPosition(sf::Vector2f(xa, ya));
    m_tail.setRotation(arrowAngle);
    m_tail.setOutlineThickness(ZOOM);
    m_tail.setOutlineColor(color);
    m_tail.setFillColor(color);
}

void ParkingDrawable::bind(Parking const& parking)
{
    m_parking = &parking;

#if 1
    m_shape.setSize(sf::Vector2f(parking.dim.length, parking.dim.width));
    m_shape.setOrigin(0.0f, m_shape.getSize().y / 2.0f);
    m_shape.setPosition(parking.position());
    m_shape.setRotation(RAD2DEG(parking.dim.angle));
    m_shape.setFillColor(sf::Color::White);
    m_shape.setOutlineThickness(ZOOM);
    m_shape.setOutlineColor(sf::Color::Black);
#else

    const float A = parking.dim.angle;
    const float W = parking.dim.width * cosf(A);
    const float h = parking.dim.length * sinf(A);
    const float x = h / tanf(A);

    m_shape.setPointCount(4);
    m_shape.setPoint(0, sf::Vector2f(0, 0));
    m_shape.setPoint(1, sf::Vector2f(x, h));
    m_shape.setPoint(2, sf::Vector2f(x + W, h));
    m_shape.setPoint(3, sf::Vector2f(W, 0));

    m_shape.setOrigin(W / 2.0f, h / 2.0f);
    m_shape.setPosition(parking.position());
    //m_shape.setRotation(RAD2DEG(parking.dim.angle));
    m_shape.setFillColor(sf::Color::White);
    m_shape.setOutlineThickness(ZOOM);
    m_shape.setOutlineColor(sf::Color::Black);
#endif
}

void ParkingDrawable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (m_parking == nullptr)
        return ;

    target.draw(m_shape, states);
}

void CarDrawable::bind(Car const& car)
{
    m_car = &car;
    CarDimension const& dim = car.dim;

    m_body_shape.setSize(sf::Vector2f(dim.length, dim.width));
    m_body_shape.setOrigin(dim.back_overhang, m_body_shape.getSize().y / 2); // Origin on the middle of the rear wheels
    m_body_shape.setFillColor(sf::Color(165, 42, 42));
    m_body_shape.setOutlineThickness(ZOOM);
    m_body_shape.setOutlineColor(sf::Color::Blue);

    m_wheel_shape.setSize(sf::Vector2f(dim.wheel_radius * 2, dim.wheel_width));
    m_wheel_shape.setOrigin(m_wheel_shape.getSize().x / 2, m_wheel_shape.getSize().y / 2);
    m_wheel_shape.setFillColor(sf::Color::Black);
    m_wheel_shape.setOutlineThickness(ZOOM);
    m_wheel_shape.setOutlineColor(sf::Color::Yellow);
}

void CarDrawable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (m_car == nullptr)
        return ;

    CarShape const& shape = m_car->shape<CarShape>();

    m_body_shape.setPosition(shape.position());
    m_body_shape.setRotation(RAD2DEG(shape.heading()));
    target.draw(m_body_shape, states);

    for (int i = 0; i < 4; ++i)
    {
        Wheel const& wheel = shape.wheel(static_cast<CarShape::WheelType>(i));

        m_wheel_shape.setPosition(wheel.position);
        m_wheel_shape.setRotation(RAD2DEG(shape.heading() + wheel.steering));
        target.draw(m_wheel_shape, states);
    }

    target.draw(Circle(m_car->position().x, m_car->position().y, 2*ZOOM, sf::Color::Black));
    TrailerDrawable td;
    for (auto const& it: m_car->trailers())
    {
        std::cout << "Dessine trailer" << std::endl;
        td.bind(*it);
        target.draw(td, states);
    }

    if (m_car->hasTrajectory())
    {
        m_car->trajectory().draw(target, states);
    }
}

void TrailerDrawable::bind(Trailer const& trailer)
{
    m_trailer = &trailer;
    TrailerDimension const& dim = trailer.dim;

    m_body_shape.setSize(sf::Vector2f(dim.length, dim.width));
    m_body_shape.setOrigin(dim.back_overhang, m_body_shape.getSize().y / 2); // Origin on the middle of the rear wheels
    m_body_shape.setFillColor(sf::Color(165, 42, 42));
    m_body_shape.setOutlineThickness(ZOOM);
    m_body_shape.setOutlineColor(sf::Color::Blue);

    m_truc_shape.setSize(sf::Vector2f(dim.wheelbase, dim.width2));
    m_truc_shape.setOrigin(0.0f, m_truc_shape.getSize().y / 2);
    m_truc_shape.setFillColor(sf::Color::Black);
    m_truc_shape.setOutlineThickness(ZOOM);
    m_truc_shape.setOutlineColor(sf::Color::Blue);

    m_wheel_shape.setSize(sf::Vector2f(dim.wheel_radius * 2, dim.wheel_width));
    m_wheel_shape.setOrigin(m_wheel_shape.getSize().x / 2, m_wheel_shape.getSize().y / 2);
    m_wheel_shape.setFillColor(sf::Color::Black);
    m_wheel_shape.setOutlineThickness(ZOOM);
    m_wheel_shape.setOutlineColor(sf::Color::Yellow);
}

void TrailerDrawable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (m_trailer == nullptr)
        return ;

    TrailerShape const& shape = m_trailer->shape<TrailerShape>();

    m_body_shape.setPosition(shape.position());
    m_body_shape.setRotation(RAD2DEG(shape.heading()));
    target.draw(m_body_shape, states);

    m_truc_shape.setPosition(shape.position());
    m_truc_shape.setRotation(RAD2DEG(shape.heading()));
    target.draw(m_truc_shape, states);

    for (int i = 0; i < 2; ++i)
    {
        Wheel const& wheel = shape.wheel(static_cast<TrailerShape::WheelType>(i));

        m_wheel_shape.setPosition(wheel.position);
        m_wheel_shape.setRotation(RAD2DEG(shape.heading() + wheel.steering));
        target.draw(m_wheel_shape, states);
    }

    target.draw(Circle(m_trailer->position().x, m_trailer->position().y, 2*ZOOM, sf::Color::Black));
}
