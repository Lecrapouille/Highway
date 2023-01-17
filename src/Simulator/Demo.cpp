//=============================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2023 Quentin Quadrat <lecrapouille@gmail.com>
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
// along with Highway.  If not, see <http://www.gnu.org/licenses/>.
//=============================================================================

#include "Simulator/Demo.hpp"
#include "ECUs/AutoParkECU/AutoParkECU.hpp"
#include "Sensors/Sensors.hpp"

//-----------------------------------------------------------------------------
//! \file "Hello simulation" demo. Show a basic simulation. An autonomous car
//! is entering in the first parking slot.
//-----------------------------------------------------------------------------

static Monitor monitor; // FIXME

//-----------------------------------------------------------------------------
//! \brief Needed simulation function: return the simulation name that will be
//! display in the windows title bar.
//-----------------------------------------------------------------------------
static const char* simulation_name()
{
    return "Hello simulation demo";
}

//-----------------------------------------------------------------------------
//! \brief "Hello simulation" demo: make the \c simulator reacts to the given
//! \c key pressed. Here the ego reacts to callbacks set with Vehicle::callback()
//! defined in the function \c customize_ego().
//-----------------------------------------------------------------------------
static void simulation_react_to(Simulator& simulator, size_t key)
{
    simulator.ego().reactTo(key);
}

//-----------------------------------------------------------------------------
//! \brief Attach sensor to the ego vehicle and bind sensor to the ECU.
//! \note the origin position of the car is the middle of the rear axle. Sensors
//! are placed relatively to the vehicle origin. X-axis is along the vehicle length
//! directed to the front. the Y-axis is along the vehicle left.
//-----------------------------------------------------------------------------
static void attach_sensors(Car& car, AutoParkECU& ecu, City const& city)
{
    // Blueprints for 4 antennas placed perpendicularly on each wheel.
    // Antenna is kind of tactile sensor like done in cockroach robots.
    // Note the blueprint is static since vehicle does not copy blueprints
    // but refer it.
    constexpr Meter range = 4.0_m; // Single detection up to this distance
    Degree orientation = 90.0_deg; // Perpendicular
    Meter offx = car.blueprint.wheelbase;
    Meter offy = car.blueprint.width / 2.0f - /*car.blueprint.wheel_width*/ 0.1_m / 2.0f; // FIXME
    static const std::map<const char*, AntennaBluePrint> antenna_blueprints = {
        { "antenna_FL", { sf::Vector2<Meter>(offx,   offy),  orientation, range } },
        { "antenna_FR", { sf::Vector2<Meter>(offx,  -offy), -orientation, range } },
        { "antenna_RL", { sf::Vector2<Meter>(0.0_m,  offy),  orientation, range } },
        { "antenna_RR", { sf::Vector2<Meter>(0.0_m, -offy), -orientation, range } },
    };

    // Attach antennas to the car.
    for (auto const& bp: antenna_blueprints)
    {
        Antenna& antenna = car.addSensor<Antenna, AntennaBluePrint>(bp.second, bp.first, city, sf::Color::Blue);
        antenna.renderable = true; // Default param. Set false to hide it
        ecu.observe(antenna); // Antenna will notify the ECU
    }

    // Blueprint for 1 radar. The radar is not yet used.
    offx = car.blueprint.wheelbase + car.blueprint.front_overhang;
    offy = 0.0_m;
    constexpr Degree fov = 45.0_deg;
    orientation = 0.0_deg;
    static const std::map<size_t, RadarBluePrint> radar_blueprints = {
        { 0u, { sf::Vector2<Meter>(offx, offy), orientation, fov, range } },
    };

    // Attach radars to the car.
    for (auto const& bp: radar_blueprints)
    {
        Radar& radar = car.addSensor<Radar, RadarBluePrint>(bp.second, "radar", city, sf::Color::Red);
        radar.renderable = true;
        // ecu.observe(radar);
    }
}

//-----------------------------------------------------------------------------
//! \brief Customize the ego vehicle. Add sensors, an ECU for doing autonomous
//! parallel maneuvers. The car reacts to the user keyboard events (drive, do
//! the parallel maneuver). We also monitor the ego car states in a CSV file
//! for post-analysis in an application such as Scilab.
//-----------------------------------------------------------------------------
static Car& customize_ego(City const& city, Car& car)
{
    // Monitor the Ego car. You can monitor other states if needed.
    monitor.open("/tmp/monitor.csv", ';');
    monitor.observe(car.position().x, car.position().y, car.speed())
           .header("Ego X-coord [m]", "Ego Y-coord [m]", "Ego longitudinal speed [mps]");

    // Add ECU for doing autonomous parking.
    // FIXME how to avoid adding car (shall be implicit)
    // FIXME avoid passing City but City.collidables() instead.
    // https://github.com/Lecrapouille/Highway/issues/26
    AutoParkECU& ecu = car.addECU<AutoParkECU>(car, city);

    // Add sensors to the ego car and bind them to the ECU.
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
//! \brief Define conditions to stop the simulation.
//-----------------------------------------------------------------------------
static bool halt_simulation_when(Simulator const& simulator)
{
    monitor.record(); // FIXME move this away

    HALT_SIMULATION_WHEN((simulator.elapsedTime() > 60.0_s),
                         "Time simulation slipped");
    HALT_SIMULATION_WHEN((simulator.ego().position().x >= 140.0_m),
                         "Ego car is outside the parking");
    HALT_SIMULATION_WHEN(simulator.ego().collided(),
                         "Ego car collided");
    CONTINUE_SIMULATION;
}

//-----------------------------------------------------------------------------
//! \brief Create a basic city world made of roads, parking slots and parked
//! cars. The ego car is on the road.
//-----------------------------------------------------------------------------
static Car& create_city(City& city)
{
    // Initial states
    std::string parking_type = "epi." + std::to_string(0u); // parallel slots
    const Meter parking_length = BluePrints::get<ParkingBluePrint>(parking_type.c_str()).length;
    const Meter parking_width = BluePrints::get<ParkingBluePrint>(parking_type.c_str()).width;
    const sf::Vector2<Meter> p(97.5_m, 105.0_m); // Initial road position
    constexpr size_t number_parkings = 5u; // Number of parking slots along the road

    // Create roads
    const Meter road_width = 2.0_m;
    const std::array<size_t, TrafficSide::Max> lanes{1u, 2u}; // Number of lanes constituing the road
    const std::vector<sf::Vector2<Meter>> road_centers = {
        p,
        p + sf::Vector2<Meter>(double(number_parkings) * parking_length, 0.0_m)
    };
    Road& road1 = city.addRoad(road_centers, road_width, lanes);

    // Create parallel parking slots
    const sf::Vector2<Meter> p1(p.x, p.y - double(lanes[TrafficSide::RightHand]) * road_width - parking_width * 0.5); // Initial position of the parking slots
    std::cout << "Parking " << p.x << ", " << p.y - double(lanes[TrafficSide::RightHand]) * road_width << std::endl;
    Parking& parking0 = city.addParking(parking_type.c_str(), p1); // FIXME .attachTo(road1, offset); => { Road::offset() }
    Parking& parking1 = city.addParking(parking_type.c_str(), parking0.position() + parking0.delta()); // FIXME .attachTo(parking0 ...
    Parking& parking2 = city.addParking(parking_type.c_str(), parking1.position() + parking1.delta());
    Parking& parking3 = city.addParking(parking_type.c_str(), parking2.position() + parking2.delta());
    city.addParking(parking_type.c_str(), parking3.position() + parking3.delta());

    // Add parked cars (static). See BluePrints.cpp for the mark of vehicle
    city.addCar("Renault.Twingo", parking0);
    city.addCar("Audi.A6", parking1);
    city.addCar("Audi.A6", parking3);

    // Self-parking ego car (dynamic).
    // Place the ego car on the begining of the 1st right-side hand of the lane (X-axis).
    // The ego is centered on its lane (Y-axis).
    return customize_ego(simulator, city, city.addEgo("Mini.Cooper", road1.offset(TrafficSide::RightHand, 0u, 0.1, 1.0)));
}

//-----------------------------------------------------------------------------
//! \brief Simulation entry point. Return functions needed by the simulator to
//! run a simulation.
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
