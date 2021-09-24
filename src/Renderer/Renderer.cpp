// 2021 Quentin Quadrat quentin.quadrat@gmail.com
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

#include "Renderer/Renderer.hpp"
#include "Utils/Utils.hpp"
#include "SelfParking/SelfParkingVehicle.hpp"
#include "World/Parking.hpp"

//------------------------------------------------------------------------------
Circle::Circle(float x, float y, float r, sf::Color color)
    : m_shape(r, 100)
{
    m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());
    m_shape.setPosition(x, y);
    m_shape.setFillColor(sf::Color(255, 255, 255, 0));
    m_shape.setOutlineThickness(ZOOM * 2.0f);
    m_shape.setOutlineColor(color);
}

//------------------------------------------------------------------------------
Arc::Arc(float x, float y, float r, float start, float end, sf::Color color)
    : m_shape(r, start, end, 100)
{
    m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());
    m_shape.setPosition(x, y);
    m_shape.setFillColor(sf::Color(255, 255, 255, 0));
    m_shape.setOutlineThickness(ZOOM * 2.0f);
    m_shape.setOutlineColor(color);
}

//------------------------------------------------------------------------------
Arrow::Arrow(const float xa, const float ya, const float xb, const float yb, sf::Color color)
{
    // Arc magnitude
    const float arrowLength = DISTANCE(xa, ya, xb, yb);

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

//------------------------------------------------------------------------------
void Renderer::draw(Parking const& parking, sf::RenderTarget& target, sf::RenderStates const& states)
{
    const float A = parking.dim.angle;
    const float l = parking.dim.length;
    const float w = parking.dim.width;

    sf::RectangleShape shape(sf::Vector2f(l, w));
    shape.setOrigin(sf::Vector2f(0, w/2));
    shape.setRotation(RAD2DEG(A));
    shape.setPosition(parking.position());
    shape.setFillColor(sf::Color::White);
    shape.setOutlineThickness(ZOOM);
    shape.setOutlineColor(sf::Color::Black);
    target.draw(shape, states);

    target.draw(Circle(parking.position().x, parking.position().y, 0.02f,
                       sf::Color::Black), states);
}

//------------------------------------------------------------------------------
void Renderer::draw(SelfParkingCar const& car, sf::RenderTarget& target, sf::RenderStates const& states)
{
    Car const& c = *reinterpret_cast<const Car*>(&car);
    draw(c, target, states);

    // Car sensors
    auto const& sensors = c.shape().sensors();
    size_t i = sensors.size();
    while (i--)
    {
        // Note: front sensors are not drawn
        if (car.turning_left() && (i == CarShape::WheelName::RL))
            target.draw(sensors[i].obb, states);
        else if (car.turning_right() && (i == CarShape::WheelName::RR))
            target.draw(sensors[i].obb, states);
    }

    // Debug Trajectory
    if (car.hasTrajectory())
    {
        car.trajectory().draw(target, states);
    }
}

//------------------------------------------------------------------------------
void Renderer::draw(Car const& car, sf::RenderTarget& target, sf::RenderStates const& states)
{
    CarShape const& car_shape = car.shape();

    // Car body.
    // Origin on the middle of the rear wheels
    sf::RectangleShape body(sf::Vector2f(car.dim.length, car.dim.width));
    body.setOrigin(car.dim.back_overhang, body.getSize().y / 2);
    body.setPosition(car_shape.position());
    body.setRotation(RAD2DEG(car_shape.heading()));
    body.setFillColor(car.color);
    body.setOutlineThickness(ZOOM);
    body.setOutlineColor(sf::Color::Blue);
    target.draw(body, states);
    target.draw(Circle(car.position().x, car.position().y, ZOOM, sf::Color::Black));

    // Car wheels
    sf::RectangleShape wheel(sf::Vector2f(car.dim.wheel_radius * 2, car.dim.wheel_width));
    wheel.setOrigin(wheel.getSize().x / 2, wheel.getSize().y / 2);
    wheel.setFillColor(sf::Color::Black);
    wheel.setOutlineThickness(ZOOM);
    wheel.setOutlineColor(sf::Color::Yellow);
    for (auto const& it: car_shape.wheels())
    {
        wheel.setPosition(it.position);
        wheel.setRotation(RAD2DEG(car_shape.heading() + it.steering));
        target.draw(wheel, states);
    }

    // TODO Turning indicator

    // Trailers
    // Origin on the middle of the rear wheels
    sf::RectangleShape arm;
    for (auto const& trailer: car.trailers())
    {
        TrailerShape const& trailer_shape = trailer->shape();

        // body
        body.setPosition(trailer_shape.position());
        body.setRotation(RAD2DEG(trailer_shape.heading()));
        body.setSize(sf::Vector2f(trailer_shape.dim.length, trailer_shape.dim.width));
        body.setOrigin(trailer_shape.dim.back_overhang, body.getSize().y / 2);
        body.setFillColor(sf::Color(165, 42, 42));
        body.setOutlineThickness(ZOOM);
        body.setOutlineColor(sf::Color::Blue);
        target.draw(body, states);
        target.draw(Circle(trailer_shape.position().x, trailer_shape.position().y,
                           ZOOM, sf::Color::Black));

        // fork
        arm.setPosition(trailer_shape.position());
        arm.setRotation(RAD2DEG(trailer_shape.heading()));
        arm.setSize(sf::Vector2f(trailer_shape.dim.wheelbase, trailer_shape.dim.fork_width));
        arm.setOrigin(0.0f, arm.getSize().y / 2);
        arm.setFillColor(sf::Color::Black);
        arm.setOutlineThickness(ZOOM);
        arm.setOutlineColor(sf::Color::Blue);
        target.draw(arm, states);

        // wheels
        wheel.setSize(sf::Vector2f(trailer_shape.dim.wheel_radius * 2,
                                   trailer_shape.dim.wheel_width));
        wheel.setOrigin(wheel.getSize().x / 2, wheel.getSize().y / 2);
        wheel.setFillColor(sf::Color::Black);
        wheel.setOutlineThickness(ZOOM);
        wheel.setOutlineColor(sf::Color::Yellow);
        for (auto const& it: trailer_shape.wheels())
        {
            wheel.setPosition(it.position);
            wheel.setRotation(RAD2DEG(trailer_shape.heading() + it.steering));
            target.draw(wheel, states);
        }
    }
}
