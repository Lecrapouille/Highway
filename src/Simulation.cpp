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
#define PLAYER_CAR_COLOR 124, 99, 197// 165, 42, 42
#define COLISION_COLOR 255, 0, 0

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
    // SFML view
    m_view = renderer().getDefaultView();
    m_view.setSize(float(application.width()), -float(application.height()));
    m_view.zoom(ZOOM);
    renderer().setView(m_view);
}

//------------------------------------------------------------------------------
void Simulation::clear()
{
    m_cars.clear();
    m_parkings.clear();
}

//------------------------------------------------------------------------------
void Simulation::createWorld(size_t angle, bool const entering)
{
    clear();

    // Create a road
    // Parking& road1 = addParking(ParkingDimensions::get("road"), sf::Vector2f(100.0f, 107.5f));

    // Create parallel or perpendicular or diagnoal parking slots
    std::string d = "epi." + std::to_string(angle);
    ParkingDimension const& dim = ParkingDimensions::get(d.c_str());
    Parking& parking0 = addParking(dim, sf::Vector2f(97.5f, 100.0f));
    Parking& parking1 = addParking(dim, parking0.position() + parking0.delta());
    Parking& parking2 = addParking(dim, parking1.position() + parking1.delta());
    Parking& parking3 = addParking(dim, parking2.position() + parking2.delta());
    Parking& parking4 = addParking(dim, parking3.position() + parking3.delta());

    // Add parked cars (static)
    Car& car0 = addCar("Renault.Twingo", parking0);
    Car& car1 = addCar("Renault.Twingo", parking2);
    std::cout << "Car0: " << car0.position().x << ", " << car0.position().y << std::endl;
    std::cout << "Car1: " << car1.position().x << ", " << car1.position().y << std::endl;

    // Self-parking car (dynamic). Always be the last one (to get it through .back())
    Car& car3 = addCar("Renault.Twingo", parking3.position() + sf::Vector2f(1.0f, 10.0f), 0.0f);
    car3.color = sf::Color(PLAYER_CAR_COLOR);

    // With trailer
    // car3.attach(TrailerDimensions::get("generic"), DEG2RAD(30.0f));

    // If leaving maneuver then force the car position and heading to be in the
    // slot.
    if (!entering)
    {
        parking1.bind(car3);
    }

    // Do the maneuver
    if (!car3.park(parking1))
    {
        std::cerr << "The car cannot park" << std::endl;
    }
}

//------------------------------------------------------------------------------
// FIXME: SFMLCar instead ?
Car& Simulation::addCar(CarDimension const& dim, sf::Vector2f const& position,
                        float const heading, float const speed, float const steering)
{
    m_cars.push_back(std::make_unique<Car>(dim));
    m_cars.back()->init(position, speed, heading, steering);
    return *m_cars.back();
}

//------------------------------------------------------------------------------
Car& Simulation::addCar(const char* model, sf::Vector2f const& position, float const heading,
                        float const speed, float const steering)
{
    return addCar(CarDimensions::get(model), position, heading, speed, steering);
}

//------------------------------------------------------------------------------
Car& Simulation::addCar(const char* model, Parking& parking)
{
    Car& car = addCar(CarDimensions::get(model), sf::Vector2f(0.0f, 0.0f), 0.0f, 0.0f, 0.0f);
    parking.bind(car);
    return car;
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
sf::Vector2f Simulation::world(sf::Vector2i const& p)
{
    return renderer().mapPixelToCoords(p);
}

//------------------------------------------------------------------------------
void Simulation::handleInput()
{
    // Measurement
    float distance;
    static sf::Vector2f P1, P2;

    sf::Event event;

    // Get the X,Y mouse coordinates from the simulated word coordinates.
    m_mouse = world(sf::Mouse::getPosition(renderer()));

    while (m_running && renderer().pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            m_running = false;
            break;
        // Get world's position
        case sf::Event::MouseButtonPressed:
            P1 = m_mouse;
            std::cout << "P1: (" << m_mouse.x << ", "
                      << m_mouse.y << ") [m]" << std::endl;
            break;
        // Measure distances in meters.
        case sf::Event::MouseButtonReleased:
            P2 = m_mouse;
            distance = SFDISTANCE(P1, P2);
            if (distance >= 0.001f)
            {
                std::cout << "P2: (" << m_mouse.x << ", "
                          << m_mouse.y << ") [m]" << std::endl;
                std::cout << "|P1P2| = " << SFDISTANCE(P1, P2)
                          << " [m]" << std::endl; 
            }
            break;
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Escape)
            {
                m_running = false;
            }
            else if (event.key.code == sf::Keyboard::A)
            {
                std::cout << "ENTERING BACKWARD PARALLEL" << std::endl;
                createWorld(0, true);
            }
            else if (event.key.code == sf::Keyboard::Z)
            {
                std::cout << "LEAVING BACKWARD PARALLEL" << std::endl;
                createWorld(0, false);
            }
            else if (event.key.code == sf::Keyboard::E)
            {
                std::cout << "ENTERING BACKWARD DIAGONAL 45" << std::endl;
                createWorld(45, true);
            }
            else if (event.key.code == sf::Keyboard::R)
            {
                std::cout << "LEAVING BACKWARD DIAGONAL 45" << std::endl;
                createWorld(45, false);
            }
            else if (event.key.code == sf::Keyboard::T)
            {
                std::cout << "ENTERING BACKWARD DIAGONAL 60" << std::endl;
                createWorld(60, true);
            }
            else if (event.key.code == sf::Keyboard::Y)
            {
                std::cout << "LEAVING BACKWARD DIAGONAL 60" << std::endl;
                createWorld(60, false);
            }
            else if (event.key.code == sf::Keyboard::U)
            {
                std::cout << "ENTERING BACKWARD DIAGONAL 75" << std::endl;
                createWorld(75, true);
            }
            else if (event.key.code == sf::Keyboard::I)
            {
                std::cout << "LEAVING BACKWARD DIAGONAL 75" << std::endl;
                createWorld(75, false);
            }
            else if (event.key.code == sf::Keyboard::O)
            {
                std::cout << "ENTERING BACKWARD PERPENDICULAR" << std::endl;
                createWorld(90, true);
            }
            else if (event.key.code == sf::Keyboard::P)
            {
                std::cout << "LEAVING BACKWARD PERPENDICULAR" << std::endl;
                createWorld(90, false);
            }
            break;
        default:
            break;
        }
    }
}

//------------------------------------------------------------------------------
void Simulation::update(const float dt) // FIXME to be threaded
{
    if (m_cars.empty())
        return ;

    Car& player = *m_cars.back();

    // Update car physics
    for (auto& it: m_cars)
    {
        // it->update(dt);

        if (it.get() != &player)
        {
            if (it->intersects(player))
            {
                std::cout << "Collide" << std::endl;
                it->color = sf::Color(COLISION_COLOR);
                player.color = sf::Color(COLISION_COLOR);
            }
            else
            {
                it->color = sf::Color(DEFAULT_CAR_COLOR);
                player.color = sf::Color(PLAYER_CAR_COLOR);
            }
        }
    }

    // Update the player's car physics
    player.update(dt);
}

//------------------------------------------------------------------------------
void Simulation::draw(const float /*dt*/)
{
    CarDrawable cd;
    ParkingDrawable pd;

    // Make the camera follows the self-parking car
    if (!m_cars.empty())
    {
        m_view.setCenter(m_cars.back()->position());
        renderer().setView(m_view);
    }

    // Draw the world
    for (auto const& it: m_parkings)
    {
        pd.bind(it);
        renderer().draw(pd);
    }

    // Draw cars
    for (auto const& it: m_cars)
    {
        cd.bind(*it);
        renderer().draw(cd);
    }
}
