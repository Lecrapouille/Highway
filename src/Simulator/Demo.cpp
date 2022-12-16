//=============================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2022 Quentin Quadrat <lecrapouille@gmail.com>
//
// This file is part of Highway.
//
// Highway is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
//=============================================================================

#include "Simulator/Demo.hpp"
#include "ECUs/AutoParkECU/AutoParkECU.hpp"

#define CONTINUE_SIMULATION false

//-----------------------------------------------------------------------------
//! \brief "Hello simulation" demo: return the simulation name.
//-----------------------------------------------------------------------------
static const char* simulation_name()
{
    return "Simple simulation demo";
}

//-----------------------------------------------------------------------------
//! \brief "Hello simulation" demo: make the simulator reacts to the given key
//! pressed.
//-----------------------------------------------------------------------------
static void simulation_react_to(Simulator& simulator, size_t key)
{
    // Allow the ego car to react to callbacks set with Vehicle::callback()
    simulator.ego().reactTo(key);
}

//-----------------------------------------------------------------------------
//! \brief "Hello simulation" demo: customize the ego vehicle.
//-----------------------------------------------------------------------------
static void add_sensors(Car& car, AutoParkECU& ecu)
{
    // 4 radars: 1 one each wheel (to make simple)
    constexpr float range = 20.0f;
    const float offx = car.blueprint.wheelbase;
    const float offy = car.blueprint.width / 2.0f - /*car.blueprint.wheel_width*/ 0.1f / 2.0f;
    static const std::map<CarBluePrint::WheelName, AntenneBluePrint> blueprints = {
        { CarBluePrint::WheelName::FL, { sf::Vector2f(offx,  offy),  90.0f, range } },
        { CarBluePrint::WheelName::FR, { sf::Vector2f(offx, -offy), -90.0f, range } },
        { CarBluePrint::WheelName::RL, { sf::Vector2f(0.0f,  offy),  90.0f, range } },
        { CarBluePrint::WheelName::RR, { sf::Vector2f(0.0f, -offy), -90.0f, range } },
    };

    for (auto const& bp: blueprints)
    {
        Antenne& antenne = car.addSensor<Antenne, AntenneBluePrint>(bp.second);
        // TODO
        // ecu.observe(antenne)
    }
}

//-----------------------------------------------------------------------------
//! \brief "Hello simulation" demo: customize the ego vehicle.
//-----------------------------------------------------------------------------
static Car& customize(City const& city, Car& car)
{
    // Add ECUs
    // FIXME how to avoid adding car (shall be implicit) and cars (pass City) ?
    AutoParkECU& ecu = car.addECU<AutoParkECU>(car, city.cars());

    // Add sensors.
    add_sensors(car, ecu);

    // Make the car reacts from the keyboard: enable the turning indicator.
    car.callback(sf::Keyboard::PageDown, [&car]()
    {
        bool turning_right = (car.turningIndicator() == TurningIndicator::Right);
        car.turningIndicator(false, turning_right ^ true);
    });

    // Make the car reacts from the keyboard: enable the turning indicator.
    car.callback(sf::Keyboard::PageUp, [&car]()
    {
        bool turning_left = (car.turningIndicator() == TurningIndicator::Left);
        car.turningIndicator(turning_left ^ true, false);
    });

    // Make the car reacts from the keyboard: set car speed (kinematic).
    car.callback(sf::Keyboard::Up, [&car]()
    {
        car.refSpeed(1.0f);
    });

    // Make the car reacts from the keyboard: make the car stopped (kinematic).
    car.callback(sf::Keyboard::Down, [&car]()
    {
        car.refSpeed(0.0f);
    });

    // Make the car reacts from the keyboard: make the car turns (kinematic).
    car.callback(sf::Keyboard::Right, [&car]()
    {
        car.refSteering(car.refSteering() - 0.1f);
    });

    // Make the car reacts from the keyboard: make the car turns (kinematic).
    car.callback(sf::Keyboard::Left, [&car]()
    {
        car.refSteering(car.refSteering() + 0.1f);
    });

    return car;
}

//-----------------------------------------------------------------------------
//! \brief "Hello simulation" demo: no condition to stop the simulation.
//-----------------------------------------------------------------------------
static bool halt_simulation_when(Simulator const& simulator)
{
    return CONTINUE_SIMULATION; // Always run the simulation
}

//-----------------------------------------------------------------------------
//! \brief "Hello simulation" demo: create a basic city world. Here made of
//! parking slots and car parked. The ego car is on the road.
//-----------------------------------------------------------------------------
static Car& create_city(City& city)
{
    // Create parallel or perpendicular or diagnoal parking slots
    const int angle = 0u;
    std::string dim = "epi." + std::to_string(angle);
    Parking& parking0 = city.addParking(dim.c_str(), sf::Vector2f(97.5f, 100.0f)); // .attachTo(road1, offset);
    Parking& parking1 = city.addParking(dim.c_str(), parking0.position() + parking0.delta());
    Parking& parking2 = city.addParking(dim.c_str(), parking1.position() + parking1.delta());
    Parking& parking3 = city.addParking(dim.c_str(), parking2.position() + parking2.delta());
    city.addParking(dim.c_str(), parking3.position() + parking3.delta());

    // Add parked cars (static)
    city.addCar("Renault.Twingo", parking0);
    city.addCar("Renault.Twingo", parking1);
    city.addCar("Renault.Twingo", parking3);

    // Self-parking car (dynamic). Always be the last in the container
    return customize(city, city.addEgo("Renault.Twingo", parking0.position() + sf::Vector2f(0.0f, 5.0f)));
}

//-----------------------------------------------------------------------------
//! \brief "Hello simulation" demo: set the scenario functions mandatory to
//! create the simulation.
//-----------------------------------------------------------------------------
Scenario simple_simulation_demo()
{
    return {
        .name = simulation_name,
        .create = create_city,
        .halt = halt_simulation_when,
        .react = simulation_react_to,
    };
}