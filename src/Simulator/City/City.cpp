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

#include "City/City.hpp"
#include <iostream>

#define COLISION_COLOR 255, 0, 0
#define CAR_COLOR 178, 174, 174
#define EGO_CAR_COLOR 124, 99, 197

//------------------------------------------------------------------------------
void City::reset()
{
    m_cars.clear();
    m_parkings.clear();
}

//------------------------------------------------------------------------------
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
void City::createWorld(size_t const angle, bool const /*parked*/)
{
    reset();

    // Create a road
    // Road& road1 = addRoad(RoadDimensions::get("road.2ways"), curvature, length);

    // Create parallel or perpendicular or diagnoal parking slots
    std::string d = "epi." + std::to_string(angle);
    ParkingBluePrint const& dim = BluePrints::get<ParkingBluePrint>(d.c_str());
    Parking& parking0 = addParking(dim, sf::Vector2f(97.5f, 100.0f)); // .attachTo(road1, offset);
    Parking& parking1 = addParking(dim, parking0.position() + parking0.delta());
    Parking& parking2 = addParking(dim, parking1.position() + parking1.delta());
    Parking& parking3 = addParking(dim, parking2.position() + parking2.delta());
    Parking& parking4 = addParking(dim, parking3.position() + parking3.delta());

    // Add parked cars (static)
    Car& car0 = addCar("Renault.Twingo", parking0);
    Car& car1 = addCar("Renault.Twingo", parking1);
    Car& car2 = addCar("Renault.Twingo", parking3);

    // Debug Renault.Twingo
    //addGhost("Renault.Twingo", sf::Vector2f(107.994f, 100.0f), 0.0f); // Em0
    //addGhost("Renault.Twingo", sf::Vector2f(109.275f, 100.193f), 0.300045f); // Em1, ThetaE1
    //addGhost("Renault.Twingo", sf::Vector2f(108.345f, 99.7811f), 0.300045f + 0.235058f); // Em2, ThetaE1 + ThetaE2

    // Debug QQ
    //addGhost("QQ", sf::Vector2f(108.405f, 100.0f), 0.0f); // Em0
    //addGhost("QQ", sf::Vector2f(109.06f, 100.042f), 0.126959f); // Em1, ThetaE1
    //addGhost("QQ", sf::Vector2f(108.59f, 99.9596f), 0.126959f + 0.0919716f); // Em2, ThetaE1 + ThetaE2

    // Self-parking car (dynamic). Always be the last in the container
    Car& ego = addEgo("Citroen.C3", parking0.position() + sf::Vector2f(0.0f, 5.0f), 0.0f, 0.0f);

#if 0
    // Make the car react to some I/O events
    ego.registerCallback(sf::Keyboard::PageDown, [](Car& ego) {
        ego.turningIndicators(false, ego.rightTurningIndicator() ^ true);
    });
    ego.registerCallback(sf::Keyboard::PageUp, [](Car& ego) {
        ego.turningIndicators(ego.leftTurningIndicator() ^ true, false);
    });
    ego.registerCallback(sf::Keyboard::Up, [](Car& ego) {
        ego.refSpeed(ego.refSpeed() + 1.0f);
    });
    ego.registerCallback(sf::Keyboard::Down, [](Car& ego) {
        ego.refSpeed(ego.refSpeed() - 1.0f);
    });
    ego.registerCallback(sf::Keyboard::Right, [](Car& ego) {
        ego.refSteering(ego.refSteering() + 1.0f);
    });
    ego.registerCallback(sf::Keyboard::Left, [](Car& ego) {
        ego.refSteering(ego.refSteering() - 1.0f);
    });
#endif

    // With trailer
    //Trailer& tr = ego.attachTrailer(TrailerDimensions::get("generic"), DEG2RAD(30.0f));
    //std::cout << tr << std::endl;
}
#pragma GCC diagnostic pop

//------------------------------------------------------------------------------
SelfParkingCar& City::addEgo(const char* model, sf::Vector2f const& position,
                             float const heading, float const speed, float const steering)
{
    m_ego = std::make_unique<SelfParkingCar>(model, sf::Color(CAR_COLOR)/*m_cars*/);
    m_ego->init(position, heading, speed, steering);
    m_ego->name = "ego";
    m_ego->color = sf::Color(EGO_CAR_COLOR);
    return *m_ego;
}

//------------------------------------------------------------------------------
Car& City::addCar(const char* model, sf::Vector2f const& position, float const heading,
                  float const speed, float const steering)
{
    static size_t count = 0u;

    m_cars.push_back(std::make_unique<Car>(model, sf::Color(CAR_COLOR)));
    m_cars.back()->init(position, heading, speed, steering);
    m_cars.back()->name += std::to_string(count++);
    return *m_cars.back();
}

//------------------------------------------------------------------------------
Car& City::addCar(const char* model, Parking& parking)
{
    Car& car = addCar(model, sf::Vector2f(0.0f, 0.0f), 0.0f, 0.0f, 0.0f);
    parking.bind(car);
    return car;
}

//------------------------------------------------------------------------------
Car& City::addGhost(const char* model, sf::Vector2f const& position, float const heading,
                    float const steering)
{
    static size_t count = 0u;
    static float speed = 0.0f;

    m_ghosts.push_back(std::make_unique<Car>(model, sf::Color(CAR_COLOR)));
    m_ghosts.back()->init(position, heading, speed, steering);
    m_ghosts.back()->name += std::to_string(count++);
    m_ghosts.back()->color = sf::Color::White;
    return *m_ghosts.back();
}

//------------------------------------------------------------------------------
Car& City::addGhost(const char* model, Parking& parking)
{
    Car& car = addGhost(model, sf::Vector2f(0.0f, 0.0f), 0.0f, 0.0f);
    parking.bind(car);
    return car;
}

//------------------------------------------------------------------------------
Parking& City::addParking(ParkingBluePrint const& dim, sf::Vector2f const& position)
{
    m_parkings.push_back(Parking(dim, position));
    return m_parkings.back();
}

//------------------------------------------------------------------------------
Parking& City::addParking(const char* type, sf::Vector2f const& position)
{
    return addParking(BluePrints::get<ParkingBluePrint>(type), position);
}
