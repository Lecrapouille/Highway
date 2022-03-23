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

#include "Renderer/Renderer.hpp"
#include "City/City.hpp"

//------------------------------------------------------------------------------
void Renderer::draw(Parking const& parking, sf::RenderTarget& target, sf::RenderStates const& states)
{
    sf::RectangleShape const& shape = parking.obb();
    target.draw(shape, states);
    target.draw(Circle(parking.position().x, parking.position().y, ZOOM, sf::Color::Black), states);
}

//------------------------------------------------------------------------------
void Renderer::draw(Autonomous const& car, sf::RenderTarget& target, sf::RenderStates const& states)
{
    Car const& c = *reinterpret_cast<const Car*>(&car);
    draw(c, target, states);

#if 0
    // Debug Trajectory
    if (car.hasTrajectory())
    {
        car.trajectory().draw(target, states);
    }
#endif
}

//------------------------------------------------------------------------------
void Renderer::draw(Car const& car, sf::RenderTarget& target, sf::RenderStates const& states)
{
    // Car body.
    sf::RectangleShape body = car.obb();
    body.setFillColor(car.color);
    body.setOutlineThickness(ZOOM);
    body.setOutlineColor(sf::Color::Blue);
    target.draw(body, states);
    target.draw(Circle(car.position().x, car.position().y, ZOOM, sf::Color::Black));

    // Car wheels
    size_t i = car.wheels().size();
    while (i--)
    {
        sf::RectangleShape wheel = car.obb_wheel(i);
        wheel.setFillColor(sf::Color::Black);
        wheel.setOutlineThickness(ZOOM);
        wheel.setOutlineColor(sf::Color::Yellow);
        target.draw(wheel, states);
    }

    // Car sensors
    for (auto const& it: car.sensors())
    {
        target.draw(it->coverageArea(), states);
    }

    // Turning indicator

    // TODO Trailers
}
