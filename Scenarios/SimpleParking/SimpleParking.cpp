//==============================================================================
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
//==============================================================================

#include "API.hpp"
#include "ECUs/AutoParkECU/AutoParkECU.hpp"

// FIXME: ajouter bool checkCity(City&) si pas de soucis => ou mieux en interne (par exemple >= 1 voiture ego)

//-----------------------------------------------------------------------------
const char* simulation_name()
{
    return "Parking simulation";
}

//-----------------------------------------------------------------------------
// TODO restart_simulation_after
bool halt_simulation_when(Simulator const& simulator)
{
    HALT_SIMULATION_WHEN((simulator.elapsedTime() > 60.0_s),
                         "Time simulation slipped");
    HALT_SIMULATION_WHEN((simulator.ego().position().x >= 140.0_m),
                         "Ego car is outside the parking");
    HALT_SIMULATION_WHEN(simulator.ego().collided(),
                         "Ego car collided");
    // TODO outside the city

    return CONTINUE_SIMULATION;
}

//-----------------------------------------------------------------------------
void react_to(Simulator& simulator, size_t const key)
{
    // Allow the ego car to react to callbacks set with Vehicle::callback()
    simulator.ego().reactTo(key);
}

//-----------------------------------------------------------------------------
// FIXME Ajouter:Car& ego_specialisation(Car&) qui est appellé par City::createEgo() { return ego_specialisation(new Car()); }
static Car& customize(Car& car)
{
    // If desired, use a different color than the default for the ego car.
    car.color = sf::Color::Cyan;

    // Add sensors
    //car.addRadar(Radar(sf::Vector2f(car.blueprint.wheelbase + car.blueprint.front_overhang, 0.0f), 90.0f, 20.0f, 2.0f));
#if 0
    car.addRadar({ .offset = sf::Vector2f(car.blueprint.wheelbase + car.blueprint.front_overhang, 0.0f),
               .orientation = 90.0f,
               .fov = 20.0f,
               .range = 2.0f/*174.0f*/ });
#endif

    // Add ECUs
    //car.addECU<AutoParkECU>(car); // FIXME how to avoid adding car ?

    // Make the car reacts from the keyboard: enable the turning indicator.
    car.callback(sf::Keyboard::PageDown, [&car]()
    {
        //car.turningIndicator(false, m_turning_right ^ true);
    });

    // Make the car reacts from the keyboard: enable the turning indicator.
    car.callback(sf::Keyboard::PageUp, [&car]()
    {
        //car.turningIndicator(m_turning_left ^ true, false);
    });

    // Make the car reacts from the keyboard: set car speed (kinematic).
    car.callback(sf::Keyboard::Up, [&car]()
    {
        car.refSpeed(1.0_mps);
    });

    // Make the car reacts from the keyboard: make the car stopped (kinematic).
    car.callback(sf::Keyboard::Down, [&car]()
    {
        car.refSpeed(0.0_mps);
    });

    // Make the car reacts from the keyboard: make the car turns (kinematic).
    car.callback(sf::Keyboard::Right, [&car]()
    {
        car.refSteering(car.refSteering() - 0.1_deg);
    });

    // Make the car reacts from the keyboard: make the car turns (kinematic).
    car.callback(sf::Keyboard::Left, [&car]()
    {
        car.refSteering(car.refSteering() + 0.1_deg);
    });

    return car;
}

//-----------------------------------------------------------------------------
Car& create_city(City& city)
{
    std::cout << "Creating city for scenario '" << simulation_name()
              << "'" << std::endl;

    // FIXME I do not understand why this is needed while Simulator::create()
    // does it before calling this instance
    BluePrints::init();

    // Create parallel or perpendicular or diagnoal parking slots
    const int angle = 0u;
    std::string dim = "epi." + std::to_string(angle);
    Parking& parking0 = city.addParking(dim.c_str(), sf::Vector2<Meter>(97.5_m, 100.0_m)); // .attachTo(road1, offset);
    Parking& parking1 = city.addParking(dim.c_str(), parking0.position() + parking0.delta());
    Parking& parking2 = city.addParking(dim.c_str(), parking1.position() + parking1.delta());
    Parking& parking3 = city.addParking(dim.c_str(), parking2.position() + parking2.delta());
    /*Parking& parking4 =*/ city.addParking(dim.c_str(), parking3.position() + parking3.delta());

    // Add parked cars (static)
    city.addCar("Renault.Twingo", parking0);
    city.addCar("Renault.Twingo", parking1);
    city.addCar("Renault.Twingo", parking3);

    // Self-parking car (dynamic). Always be the last in the container
    return customize(city.addEgo("Renault.Twingo", parking0.position() + sf::Vector2<Meter>(0.0_m, 5.0_m)));
}
