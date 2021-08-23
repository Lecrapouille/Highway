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

#include "Simulation.hpp"
#include "Dimensions.hpp"
#include "Renderer.hpp"
#include <iostream>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000

//------------------------------------------------------------------------------
// Note the view coordinates are
//   ^ Y
//   |
//   +------> X
// Changed through view.setSize() to follow the same computations than the doc
// "Estimation et controle pour le pilotage automatique de vehicule" by Sungwoo Choi
Simulation::Simulation(Application& application)
    : GUIStates("Car Simulation", application.renderer())
{
    m_view = renderer().getDefaultView();
    m_view.setSize(float(application.width()), -float(application.height()));
    m_view.zoom(ZOOM);
    renderer().setView(m_view);

    ParkingDimension const& dim = ParkingDimensions::get("creneau");

    Parking& parking = addParking(dim, sf::Vector2f(97.5f, 100.0f));
    addParking(dim, parking.position() + sf::Vector2f(parking.dim.length, 0.0f));

    Car& car = addCar("Renault.Twingo", sf::Vector2f(105.0f, 102.0f), 0.0f);
    //parking.bind(car);

    // Final destination
    if (!car.park(parking.position()))
    {
       std::cerr << "The car cannot park" << std::endl;
    }
}

//------------------------------------------------------------------------------
Car& Simulation::addCar(CarDimension const& dim, sf::Vector2f const& position, float const heading,
                        float const speed, float const steering)
{
    m_cars.push_back(Car(dim));
    m_cars.back().init(position, speed, heading, steering);
    return m_cars.back();
}

//------------------------------------------------------------------------------
Car& Simulation::addCar(const char* model, sf::Vector2f const& position, float const heading,
                        float const speed, float const steering)
{
    return addCar(CarDimensions::get(model), position, heading, speed, steering);
}

//------------------------------------------------------------------------------
Parking& Simulation::addParking(ParkingDimension const& dim, sf::Vector2f const& position)
{
    // TODO i = count; while (i--)
    m_parkings.push_back(Parking(dim, position));
    return m_parkings.back();
}

//------------------------------------------------------------------------------
Parking& Simulation::addParking(const char* type, sf::Vector2f const& position)
{
    return addParking(ParkingDimensions::get(type), position);
}

//------------------------------------------------------------------------------
void Simulation::handleInput()
{
    sf::Event event;

    while (m_running && renderer().pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            m_running = false;
            break;
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Escape)
            {
                m_running = false;
            }
            break;
        default:
            break;
        }
    }
}

//------------------------------------------------------------------------------
void Simulation::update(const float dt)
{
    for (auto& it: m_cars)
    {
        it.update(dt);
    }
}

//------------------------------------------------------------------------------
void Simulation::draw(const float /*dt*/)
{
    CarDrawable cd;
    ParkingDrawable pd;

    m_view.setCenter(m_cars[0].position());
    renderer().setView(m_view);

    for (auto const& it: m_parkings)
    {
        pd.bind(it);
        renderer().draw(pd);
    }

    for (auto const& it: m_cars)
    {
        cd.bind(it);
        renderer().draw(cd);
    }
}
