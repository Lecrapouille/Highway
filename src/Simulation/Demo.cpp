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

#include "Demo.hpp"
#include "ECUs/AutoParkECU/AutoParkECU.hpp"
#include "City/CityGenerator.hpp"

//-----------------------------------------------------------------------------
//! \file "Hello simulation" demo. Show a basic simulation. An autonomous car
//! is entering in the first parking slot.
//-----------------------------------------------------------------------------

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
        //antenna.renderable = true; // Default param. Set false to hide it
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
        //radar.renderable = true;
        // ecu.observe(radar);
    }

    car.showSensors(false);
}

//-----------------------------------------------------------------------------
//! \brief Customize the ego vehicle. Add sensors, an ECU for doing autonomous
//! parallel maneuvers. The car reacts to the user keyboard events (drive, do
//! the parallel maneuver). We also monitor the ego car states in a CSV file
//! for post-analysis in an application such as Scilab.
//-----------------------------------------------------------------------------
static Car& customize_ego(Simulator& simulator, City const& city, Car& car)
{
    // Monitor the Ego car. You can monitor other states if needed.
    simulator.monitor.observe(car.position().x, car.position().y, car.speed())
            .header("Ego X-coord [m]", "Ego Y-coord [m]", "Ego longitudinal speed [mps]");

    // Add ECU for doing autonomous parking.
    // FIXME how to avoid adding car (shall be implicit)
    // FIXME avoid passing City but City.collidables() instead.
    // https://github.com/Lecrapouille/Highway/issues/26
    AutoParkECU& ecu = car.addECU<AutoParkECU>(car, city);

    // Display ECU message to the GUI
    ecu.setListener(simulator);

    // Add sensors to the ego car and bind them to the ECU.
    attach_sensors(car, ecu, city);

    // Make the car reacts from the keyboard: enable the turning indicator.
    car.callback(sf::Keyboard::PageDown, [&car]()
    {
        car.turningIndicator.down();
    });

    // Make the car reacts from the keyboard: enable the turning indicator.
    car.callback(sf::Keyboard::PageUp, [&car]()
    {
        car.turningIndicator.up();
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
static bool simulation_halt_when(Simulator const& simulator)
{/*
    HALT_SIMULATION_WHEN((simulator.elapsedTime() > 60.0_s),
                         "Time simulation slipped!");
    HALT_SIMULATION_WHEN((simulator.ego().position().x >= 140.0_m),
                         "Ego car is outside the parking!");
    HALT_SIMULATION_WHEN(simulator.ego().collided(),
                         "Ego car collided!");*/
    CONTINUE_SIMULATION;
}

//-----------------------------------------------------------------------------
//! \brief Create a basic city world made of roads, parking slots and parked
//! cars. The ego car is on the road.
//-----------------------------------------------------------------------------
static Car& simulation_create_city(Simulator& simulator, City& city)
{
    CityGenerator g;
    auto const& seg = g.generate(sf::Vector2<Meter>(5.0_km, 5.0_km));
    for (auto const &it: seg)
    {
        if (it->highway)
            city.addRoad({ it->from, it->to }, 2.5_m, {2u, 2u});
        else
            city.addRoad({ it->from, it->to }, 2.0_m, {1u, 1u});
    }

    Car& ego = city.addEgo("Renault.Twingo", *city.roads()[0], TrafficSide::RightHand, 0u, 0.0, 0.5);
    return customize_ego(simulator, city, ego);

#if 0
    // Initial states
    const char *parking_type = "epi.0"; // parallel slots
    const Meter parking_length = BluePrints::get<ParkingBluePrint>(parking_type).length;
    const Meter parking_width = BluePrints::get<ParkingBluePrint>(parking_type).width;
    const sf::Vector2<Meter> p(97.0_m, 105.0_m); // Initial road position
    constexpr size_t number_parkings = 5u; // Number of parking slots along the road

    // Create a road
    const Meter road_width = 2.0_m;
    const Meter road_distance = double(number_parkings) * parking_length;
    const std::vector<sf::Vector2<Meter>> road_centers1 = {
        p, p + sf::Vector2<Meter>(road_distance, road_distance)
    };
    const std::vector<sf::Vector2<Meter>> road_centers2 = {
        sf::Vector2<Meter>(105.753_m, 149.745_m), sf::Vector2<Meter>(156.371_m, 103.166_m)
    };
    Road& road1 = city.addRoad(road_centers1, road_width, {1u, 2u});
    Road& road2 = city.addRoad(road_centers2, road_width, {2u, 2u});

    // Add cars along the road.
    city.addCar("Renault.Twingo", road1, TrafficSide::LeftHand, 0u, 0.0, 0.5);

    // Create parallel parking slots along the road right side
    city.addParking(parking_type, road1, TrafficSide::LeftHand, 0.0, 1.0);
    Parking& parking0 = city.addParking(parking_type, road1, TrafficSide::RightHand, 0.0, 1.0);
    Parking& parking1 = city.addParking(parking0);
    Parking& parking2 = city.addParking(parking1);
    Parking& parking3 = city.addParking(parking2);
    Parking& parking4 = city.addParking(parking3);

    // Add parked cars (static). See BluePrints.cpp for the mark of vehicle.
    // Parking slots 2 and 4 are empty.
    city.addCar("Renault.Twingo", parking0);
    city.addCar("Renault.Twingo", parking1);
    city.addCar("Renault.Twingo", parking3);

    // Self-parking ego car (dynamic).
    // Place the ego car on the begining of the 1st right-side hand of the lane (X-axis).
    // The ego is centered on its lane (Y-axis).
    Car& ego = city.addEgo("Renault.Twingo", road1, TrafficSide::RightHand, 0u, 0.0, 0.5);
    return customize_ego(simulator, city, ego);
#endif
}

//-----------------------------------------------------------------------------
//! \brief Simulation entry point. Return functions needed by the simulator to
//! run a simulation.
//-----------------------------------------------------------------------------
Scenario simple_simulation_demo()
{
    return {
        .name = simulation_name,
        .create = simulation_create_city,
        .halt = simulation_halt_when,
        .react = simulation_react_to,
    };
}
