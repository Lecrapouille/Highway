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

#include "Simulation.hpp"
#include "Renderer/Renderer.hpp"

#define DEFAULT_CAR_COLOR DEFAULT_VEHICLE_COLOR
#define EGO_CAR_COLOR 124, 99, 197
#define COLISION_COLOR 255, 0, 0

//------------------------------------------------------------------------------
void Simulation::clear()
{
    m_cars.clear();
    m_parkings.clear();
}

//------------------------------------------------------------------------------
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
void Simulation::createWorld(size_t angle, bool const entering)
{
    clear();

    // Create a road
    // Road& road1 = addRoad(RoadDimensions::get("road.2ways"), curvature, length);

    // Create parallel or perpendicular or diagnoal parking slots
    std::string d = "epi." + std::to_string(angle);
    ParkingDimension const& dim = ParkingDimensions::get(d.c_str());
    Parking& parking0 = addParking(dim, sf::Vector2f(97.5f, 100.0f)); // .attachTo(road1, offset);
    Parking& parking1 = addParking(dim, parking0.position() + parking0.delta());
    Parking& parking2 = addParking(dim, parking1.position() + parking1.delta());
    Parking& parking3 = addParking(dim, parking2.position() + parking2.delta());
    Parking& parking4 = addParking(dim, parking3.position() + parking3.delta());

    // Add parked cars (static)
    Car& car0 = addCar("Renault.Twingo", parking0);
    Car& car1 = addCar("Audi.A6", parking1);
    Car& car2 = addCar("Audi.A6", parking3);

    // Self-parking car (dynamic). Always be the last in the container
    Car& ego = addEgo("Renault.Twingo", parking0.position() + sf::Vector2f(0.0f, 2.0f), 0.0f);

    // With trailer
    //Trailer& tr = ego.attachTrailer(TrailerDimensions::get("generic"), DEG2RAD(30.0f));
    //std::cout << tr << std::endl;
}
#pragma GCC diagnostic pop

//------------------------------------------------------------------------------
SelfParkingCar& Simulation::addEgo(CarDimension const& dim, sf::Vector2f const& position,
                             float const heading, float const speed, float const steering)
{
    m_ego = std::make_unique<SelfParkingCar>(dim, m_cars);
    m_ego->init(position, heading, speed, steering);
    m_ego->name = "ego";
    m_ego->color = sf::Color(EGO_CAR_COLOR);
    std::cout << *m_ego << std::endl << std::endl;
    return *m_ego;
}

//------------------------------------------------------------------------------
SelfParkingCar& Simulation::addEgo(const char* model, sf::Vector2f const& position,
                             float const heading, float const speed, float const steering)
{
    return addEgo(CarDimensions::get(model), position, heading, speed, steering);
}

//------------------------------------------------------------------------------
Car& Simulation::addCar(CarDimension const& dim, sf::Vector2f const& position,
                        float const heading, float const speed, float const steering)
{
    static size_t count = 0u;

    m_cars.push_back(std::make_unique<Car>(dim));
    m_cars.back()->init(position, heading, speed, steering);
    m_cars.back()->name += std::to_string(count++);
    std::cout << *m_cars.back() << std::endl << std::endl;
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
Parking& Simulation::addParking(ParkingDimension const& dim, sf::Vector2f const& position) //TODO , count)
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
void Simulation::update(const float dt)
{
    // Update car physics
    for (auto& it: m_cars)
    {
        it->update(dt);
    }

    if (m_ego != nullptr)
    {
        // Update the Ego's car physics
        m_ego->update(dt);

        // Collide with other car ?
        for (auto& it: m_cars)
        {
            if (m_ego->collides(*it))
            {
                std::cout << "Collide" << std::endl;
                it->color = sf::Color(COLISION_COLOR);
                m_ego->color = sf::Color(COLISION_COLOR);
            }
            else
            {
                it->color = sf::Color(DEFAULT_CAR_COLOR);
                m_ego->color = sf::Color(EGO_CAR_COLOR);
            }
        }
    }
}

//------------------------------------------------------------------------------
void Simulation::draw(sf::RenderWindow& renderer, sf::View& view)
{
    // Make the camera follows the self-parking car
    if (m_ego != nullptr)
    {
        view.setCenter(m_ego->position());
        renderer.setView(view);
    }

    // Draw the world
    for (auto const& it: m_parkings)
    {
        Renderer::draw(it, renderer);
    }

    // Draw cars
    for (auto const& it: m_cars)
    {
        Renderer::draw(*it, renderer);
    }

    if (m_ego != nullptr)
    {
        Renderer::draw(*m_ego, renderer);
    }
}
