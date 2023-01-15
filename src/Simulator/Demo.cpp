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
#include "Sensors/Sensors.hpp"

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
static void attach_sensors(Car& car, AutoParkECU& ecu, City const& city)
{
    // Blueprints for 4 antennas: 1 placed on each wheel.
    // Note shall be static since vehicle does not copy blueprints.
    constexpr Meter range = 4.0_m;
    Degree orientation = 90.0_deg;
    Meter offx = car.blueprint.wheelbase;
    Meter offy = car.blueprint.width / 2.0f - /*car.blueprint.wheel_width*/ 0.1_m / 2.0f; // FIXME
    static const std::map<const char*, AntennaBluePrint> antenna_blueprints = {
        { "antenna_FL", { sf::Vector2<Meter>(offx,   offy),  orientation, range } },
        { "antenna_FR", { sf::Vector2<Meter>(offx,  -offy), -orientation, range } },
        { "antenna_RL", { sf::Vector2<Meter>(0.0_m,  offy),  orientation, range } },
        { "antenna_RR", { sf::Vector2<Meter>(0.0_m, -offy), -orientation, range } },
    };

    // Attach antennas to the car
    for (auto const& bp: antenna_blueprints)
    {
        Antenna& antenna = car.addSensor<Antenna, AntennaBluePrint>(bp.second, bp.first, city, sf::Color::Blue);
        antenna.renderable = true;
        ecu.observe(antenna);
    }

    // Blueprint for 1 radar.
    // Note shall be static since vehicle does not copy blueprints.
    offx = car.blueprint.wheelbase + car.blueprint.front_overhang;
    offy = 0.0_m;
    constexpr Degree fov = 45.0_deg;
    orientation = 0.0_deg;
    static const std::map<size_t, RadarBluePrint> radar_blueprints = {
        { 0u, { sf::Vector2<Meter>(offx, offy), orientation, fov, range } },
    };

    // Attach radars to the car
    for (auto const& bp: radar_blueprints)
    {
        Radar& radar = car.addSensor<Radar, RadarBluePrint>(bp.second, "radar", city, sf::Color::Red);
        radar.renderable = true;
        // ecu.observe(radar);
    }
}

//-----------------------------------------------------------------------------
//! \brief "Hello simulation" demo: customize the ego vehicle.
//-----------------------------------------------------------------------------
static Car& customize(City const& city, Car& car)
{
    // Add ECUs
    // FIXME how to avoid adding car (shall be implicit)
    // FIXME avoid passing and cars needed for sensors (pass instead City or World.collidable()) ?
    // https://github.com/Lecrapouille/Highway/issues/26
    AutoParkECU& ecu = car.addECU<AutoParkECU>(car, city);

    // Add sensors.
    attach_sensors(car, ecu, city);

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
        car.refSteering(car.refSteering() - 1.0_deg);
    });

    // Make the car reacts from the keyboard: make the car turns (kinematic).
    car.callback(sf::Keyboard::Left, [&car]()
    {
        car.refSteering(car.refSteering() + 1.0_deg);
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
    std::string parking_type = "epi." + std::to_string(0u); // parallel slots
    const Meter parking_length = BluePrints::get<ParkingBluePrint>(parking_type.c_str()).length;
    const Meter road_width = 2.5_m;
    const sf::Vector2<Meter> p(97.5_m, 100.0_m); // Initial position of the road
    const std::array<size_t, TrafficSide::Max> lanes{1u, 1u}; // Number of lanes constituing the road
    constexpr size_t number_parkings = 4u; // Number of parking slots along the road

    // Create roads
    Road& road1 = city.addRoad(p, sf::Vector2<Meter>(p.x + + float(number_parkings) * parking_length, p.y),
                               road_width, lanes);

    // Create parallel or perpendicular or diagnoal parking slots
    const sf::Vector2<Meter> p1(p.x + double(lanes[TrafficSide::RightHand]) * road_width, p.y); // Initial position of the parking slots
    Parking& parking0 = city.addParking(parking_type.c_str(), p1); // FIXME .attachTo(road1, offset); => { Road::offset() }
    Parking& parking1 = city.addParking(parking_type.c_str(), parking0.position() + parking0.delta()); // FIXME .attachTo(parking0 ...
    Parking& parking2 = city.addParking(parking_type.c_str(), parking1.position() + parking1.delta());
    Parking& parking3 = city.addParking(parking_type.c_str(), parking2.position() + parking2.delta());
    city.addParking(parking_type.c_str(), parking3.position() + parking3.delta());

    // Add parked cars (static). See BluePrints.cpp for mark of vehicle
    city.addCar("Renault.Twingo", parking0);
    city.addCar("Audi.A6", parking1);
    city.addCar("Audi.A6", parking3);

    // Self-parking car (dynamic). Always be the last in the container
    return customize(city, city.addEgo("Mini.Cooper", parking0.position() + sf::Vector2<Meter>(0.0_m, 5.0_m)));
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
