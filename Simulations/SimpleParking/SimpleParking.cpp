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

#include "Simulation.h"
#include "ECUs/AutoParkECU/AutoParkECU.hpp"

//-----------------------------------------------------------------------------
const char* simulation_name()
{
    return "Parking simulation";
}

//-----------------------------------------------------------------------------
bool halt_simulation_when(Simulator const& simulator)
{
    return simulator.elapsedTime() > sf::seconds(30.0f);
}

//-----------------------------------------------------------------------------
static Car& customize(Car& car)
{
    // Add sensors
    //car.addRadar(Radar(sf::Vector2f(car.blueprint.wheelbase + car.blueprint.front_overhang, 0.0f), 90.0f, 20.0f, 2.0f));
#if 0
    car.addRadar({ .offset = sf::Vector2f(car.blueprint.wheelbase + car.blueprint.front_overhang, 0.0f),
               .orientation = 90.0f,
               .fov = 20.0f,
               .range = 2.0f/*174.0f*/ });
#endif

    // Add behaviors
    car.addECU<AutoParkECU>(car); // FIXME how to avoid adding car ?

    // Add reactions from keyboard
    car.registerCallback(sf::Keyboard::PageDown, [&car]()
    {
        //car.turningIndicator(false, m_turning_right ^ true);
    });

    car.registerCallback(sf::Keyboard::PageUp, [&car]()
    {
        //car.turningIndicator(m_turning_left ^ true, false);
    });

    car.registerCallback(sf::Keyboard::Up, [&car]()
    {
        car.refSpeed(1.0f);
    });

    car.registerCallback(sf::Keyboard::Down, [&car]()
    {
        car.refSpeed(0.0f);
    });

    car.registerCallback(sf::Keyboard::Right, [&car]()
    {
        car.refSteering(car.refSteering() - 0.1f);
    });

    car.registerCallback(sf::Keyboard::Left, [&car]()
    {
        car.refSteering(car.refSteering() + 0.1f);
    });

    return car;
}

//-----------------------------------------------------------------------------
Car& create_city(City& city)
{
    std::cout << "Creating city for " << simulation_name() << std::endl;

    BluePrints::init(); // FIXME
    city.reset();

    // Create parallel or perpendicular or diagnoal parking slots
    const int angle = 0u;
    std::string dim = "epi." + std::to_string(angle);
    Parking& parking0 = city.addParking(dim.c_str(), sf::Vector2f(97.5f, 100.0f)); // .attachTo(road1, offset);
    Parking& parking1 = city.addParking(dim.c_str(), parking0.position() + parking0.delta());
    Parking& parking2 = city.addParking(dim.c_str(), parking1.position() + parking1.delta());
    Parking& parking3 = city.addParking(dim.c_str(), parking2.position() + parking2.delta());
    /*Parking& parking4 =*/ city.addParking(dim.c_str(), parking3.position() + parking3.delta());

    // Add parked cars (static)
    city.addCar("Renault.Twingo", parking0);
    city.addCar("Renault.Twingo", parking1);
    city.addCar("Renault.Twingo", parking3);

    // Self-parking car (dynamic). Always be the last in the container
    return customize(city.addEgo("Renault.Twingo", parking0.position() + sf::Vector2f(0.0f, 5.0f)));
}
