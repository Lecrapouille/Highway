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

#include "Highway/Scenario.hpp"

//-----------------------------------------------------------------------------
//! \file Show the template for creating a scenario.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//! \brief Mandatory function. Return the scenario name that will be displayed
//! by the GUI (i.e. the windows title bar).
//-----------------------------------------------------------------------------
const char* scenario_name()
{
    return "Hello template scenario";
}

//-----------------------------------------------------------------------------
//! \brief Mandatory function. Define conditions for stopping the scenario. For
//! Example: simulation time up, vehicle collisions, out of scenario bounds are
//! conditions of failures. Reach the final destination is a condition of success.
//! Else, the scenario is not finished and the simulation continues.
//-----------------------------------------------------------------------------
/*Scenario::Status*/ int scenario_halt_when(Simulator const& simulator)
{
    SCENARIO_FAILS_WHEN((simulator.elapsedTime() > 60.0_s),
        "Time scenario slipped!");
    SCENARIO_SUCCEEDS_WHEN((simulator.ego().position().x >= 140.0_m),
        "Ego car left the road with success!");
    //SCENARIO_FAILS_WHEN((simulator.ego().collided()),
    //    "Ego car collided!");
    SCENARIO_CONTINUES;
}

//-----------------------------------------------------------------------------
//! \brief Mandatory function. Make the \c simulator reacts to the incoming \c
//! event. In our case, the ego vehicle reacts to callbacks configured with
//! Vehicle::callback() defined in the function \c customize_ego().
//-----------------------------------------------------------------------------
void scenario_react_to(Simulator& simulator, size_t event)
{
    simulator.ego().reactTo(event);
}

//-----------------------------------------------------------------------------
//! \brief Locally private scenario function. Customize the ego vehicle. In our
//! current case, we make our vehicle reacts to keyboard by applying reference
//! speed. Currently the vehicle physic uses the bycicle kinematic model.
//-----------------------------------------------------------------------------
static Car& customize_ego(Simulator& simulator, City const& city, Car& ego)
{
    // Make the ego reacts from the keyboard: set ego speed (kinematic).
    ego.addCallback(sf::Keyboard::Up, [&ego]()
    {
        //ego.refSpeed(1.0_mps);
    });

    // Make the ego reacts from the keyboard: make the ego stopped (kinematic).
    ego.addCallback(sf::Keyboard::Down, [&ego]()
    {
        //ego.refSpeed(0.0_mps);
    });

    // Make the ego reacts from the keyboard: make the ego turns (kinematic).
    ego.addCallback(sf::Keyboard::Right, [&ego]()
    {
        //ego.refSteering(ego.refSteering() - 1.0_deg);
    });

    // Make the ego reacts from the keyboard: make the ego turns (kinematic).
    ego.addCallback(sf::Keyboard::Left, [&ego]()
    {
        //ego.refSteering(ego.refSteering() + 1.0_deg);
    });

    return ego;
}

//-----------------------------------------------------------------------------
//! \brief Mandatory function. Create a basic city made of a single road and our
//! ego vehicle. The ego car is on the road. The function shall return our ego
//! vehicle.
//-----------------------------------------------------------------------------
Car& scenario_create_city(Simulator& simulator, City& city)
{
//std::cout << BluePrints::/*instance().*/get<vehicle::BluePrint>("Renault.Twingo").length << std::endl;

    Car& ego = city.addEgo("Mini.Cooper", { 0.0_m, 0.0_m }, 0.0_deg, 0.0_mps, 0.0_deg);
    return customize_ego(simulator, city, ego);

#if 0
    // Create a single road (2-lanes, 1-lane).
    Road& road1 = city.addRoad(
        { 0.0_m, 0.0_m,  },
        2.0_m,   // lane width,
        {1u, 2u} // 1 lane for the left side and 2 lanes for the right.
    );

    // Add car along the road: placed on the 0th left lane, positioned on the starting
    // position of the road and centered on its lane (50%).
    city.addCar("Mini.Cooper", road1, TrafficDirection::Left, 0u, 0.0, 0.5);
    
    // Add the ego along the road: placed on the 0th right lane, positioned 10% after
    // the starting position of the road and centered on its lane (50%).
    Car& ego = city.addEgo("Mini.Cooper", road1, TrafficDirection::Right, 0u, 0.1, 0.5);

    return customize_ego(simulator, city, ego);
#endif
}
