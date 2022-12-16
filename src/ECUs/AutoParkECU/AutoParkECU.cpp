//=====================================================================
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
//=====================================================================

#include "ECUs/AutoParkECU/AutoParkECU.hpp"
#include "Simulator/Vehicle/Car.hpp"
#include "Simulator/City/Parking.hpp"
#include <iostream>

//-------------------------------------------------------------------------
AutoParkECU::Scanner::Status
AutoParkECU::Scanner::update(float const dt, Car& car, bool detected)
{
    States state = m_state;

    // This condition is purely for the simulation: is the ego car outside the
    // simulation game (parking area) ?
    if (car.position().x >= 140.0f) // out of parking
    {
        std::cout << "  Outside parking" << std::endl;
        m_state = AutoParkECU::Scanner::States::EMPTY_SPOT_NOT_FOUND;
    }

    switch (m_state)
    {
    case AutoParkECU::Scanner::States::EMPTY_SPOT_NOT_FOUND:
        // The car did not found the parking spot: reset states.
        car.refSpeed(0.0f);
        return AutoParkECU::Scanner::Status::FAILED;

    case AutoParkECU::Scanner::States::IDLE:
        // The car was stopped and now it has to drive along parking spots and
        // scan parked cars to find the first empty parking spot.
        car.refSpeed(2.0f);
        car.refSteering(0.0f);
        m_distance = 0.0f;
        m_state = AutoParkECU::Scanner::States::DETECT_FIRST_CAR;
        return AutoParkECU::Scanner::Status::IN_PROGRESS;

    case AutoParkECU::Scanner::States::DETECT_FIRST_CAR:
        // The car has detected the first parked car. Now search a gap (either
        // space between car either real parking spot).
        if (!detected) // FIXME bitfield
        {
            m_position = car.position();
            m_state = AutoParkECU::Scanner::States::DETECT_EMPTY_SPOT;
        }
        return AutoParkECU::Scanner::Status::IN_PROGRESS;

    case AutoParkECU::Scanner::States::DETECT_EMPTY_SPOT:
        // The car is detecting a "hole". Is it a real parking spot ? To know it
        // it integrates its speed to know the length of the spot.
        if (detected)
        {
            m_state = AutoParkECU::Scanner::States::DETECT_SECOND_CAR;
        }
        else if (m_distance >= 6.4f) // meters FIXME should be Lmin
        {
            // two consecutive empty spots: avoid to drive to the next parked
            // car do the maneuver directly
            m_state = AutoParkECU::Scanner::States::DETECT_SECOND_CAR;
        }

        m_distance += car.speed() * dt;
        return AutoParkECU::Scanner::Status::IN_PROGRESS;

    case AutoParkECU::Scanner::States::DETECT_SECOND_CAR:
        if (detected && (m_distance <= car.blueprint.length))
        {
            // Too small length: continue scanning parked cars
            std::cout << "Scan: No way to park at X: " << m_position.x
                      << " because distance is too short (" << m_distance << " m)" << std::endl;
            m_state = AutoParkECU::Scanner::States::DETECT_FIRST_CAR;
        }
        else if (detected || m_distance >= 6.4f) // FIXME should be Lmin
        {
            car.refSpeed(0.0f);

            // TODO Missing detection of the type of parking type. FIXME 2.0f: parking width
            ParkingBluePrint dim(m_distance, 2.0f, 0u);
            m_parking = std::make_unique<Parking>(dim, sf::Vector2f(m_position.x, m_position.y - 5.0f)); // FIXME calculer la profondeur
            std::cout << "Scan: Parking spot detected: " << *m_parking << std::endl;
            m_state = AutoParkECU::Scanner::States::EMPTY_SPOT_FOUND;
            return AutoParkECU::Scanner::Status::SUCCEEDED;
        }
        m_state = AutoParkECU::Scanner::States::IDLE;
        return AutoParkECU::Scanner::Status::IN_PROGRESS;

    case AutoParkECU::Scanner::States::EMPTY_SPOT_FOUND:
        // The parking was found, stay in this state and return the parking
        car.refSpeed(0.0f);
        return AutoParkECU::Scanner::Status::SUCCEEDED;
    }

    // Debug purpose
    if (state != m_state)
    {
        std::cout << "  SelfParkingCar::Scan new state: "
                  << AutoParkECU::Scanner::to_string(m_state) << std::endl;
    }

    return AutoParkECU::Scanner::Status::FAILED;
}

//-------------------------------------------------------------------------
void AutoParkECU::StateMachine::update(float const dt, AutoParkECU& ecu)
{
    States state = m_state;
    bool detected = ecu.detect();

    // Has the driver aborted the auto-parking system ?
    if ((m_state != AutoParkECU::StateMachine::States::IDLE) &&
         (ecu.m_ego.turningIndicator() == TurningIndicator::Off))
    {
        std::cout << "The driver has aborted the auto-parking" << std::endl;
        m_state = AutoParkECU::StateMachine::States::TRAJECTORY_DONE;
    }

    switch (m_state)
    {
    case AutoParkECU::StateMachine::States::IDLE:
        // Waiting the driver press a button to start the self-parking process.
        if ((ecu.m_ego.turningIndicator() == TurningIndicator::Left) || 
            (ecu.m_ego.turningIndicator() == TurningIndicator::Right))
        {
            if (true) // TODO car.isParked()
            {
                // The car is not parked: start scanning parked cars
                m_state = AutoParkECU::StateMachine::States::SCAN_PARKING_SPOTS;
                m_scanner.start();
            }
            else
            {
                // The car was parked: compute the path to leave the spot
                m_state = AutoParkECU::StateMachine::States::COMPUTE_LEAVING_TRAJECTORY;
            }
        }
        break;

    case AutoParkECU::StateMachine::States::SCAN_PARKING_SPOTS:
        {
            // The car is scanning parked cars to find an empty parking spot
            AutoParkECU::Scanner::Status scanning = m_scanner.update(dt, ecu.m_ego, detected);

            // Empty parking spot detected
            if (scanning == AutoParkECU::Scanner::Status::SUCCEEDED)
            {
                m_state = AutoParkECU::StateMachine::States::COMPUTE_ENTERING_TRAJECTORY;
            }
            // Failed to find an empty parking spot
            else if (scanning == AutoParkECU::Scanner::Status::FAILED)
            {
                m_state = AutoParkECU::StateMachine::States::TRAJECTORY_DONE;
            }
            else // scanning == Scan::Status::IN_PROGRESS
            {
                // Do nothing: the car is still scanning parked car to find
                // empty parking spot.
            }
        }
        break;

    case AutoParkECU::StateMachine::States::COMPUTE_ENTERING_TRAJECTORY:
        // Empty parking spot detected: if possible compute a path to the spot
        if (ecu.park(m_scanner.parking(), true))
        {
            m_state = AutoParkECU::StateMachine::States::DRIVE_ALONG_TRAJECTORY;
        }
        else
        {
            m_state = AutoParkECU::StateMachine::States::IDLE;
        }
        break;

    case AutoParkECU::StateMachine::States::COMPUTE_LEAVING_TRAJECTORY:
        // Leaving the parking spot detected: if possible compute a path to exit
        // the spot.
        if (ecu.park(m_scanner.parking(), false))
        {
            m_state = AutoParkECU::StateMachine::States::DRIVE_ALONG_TRAJECTORY;
        }
        else
        {
            std::cout << "SORRY I do not know how to leave by myself" << std::endl;
            m_state = AutoParkECU::StateMachine::States::TRAJECTORY_DONE;
        }
        break;

    case AutoParkECU::StateMachine::States::DRIVE_ALONG_TRAJECTORY:
        // The car is driving along its computed path
        if (!ecu.hasTrajectory())
        {
            std::cout << "No trajectory" << std::endl;
            m_state = AutoParkECU::StateMachine::States::TRAJECTORY_DONE;
        }
        else if (ecu.updateTrajectory(dt) == false)
        {
            std::cout << "Trajectory done" << std::endl;
            m_state = AutoParkECU::StateMachine::States::TRAJECTORY_DONE;
        }
        else
        {
            // Do nothing: the car is currently travelling along the trajectory
        }
        break;

    case AutoParkECU::StateMachine::States::TRAJECTORY_DONE:
        // Reset the car states
        m_state = AutoParkECU::StateMachine::States::IDLE;
        ecu.m_ego.turningIndicator(false, false);
        ecu.m_ego.refSpeed(0.0f);
        break;
    }

    // Debug purpose
    if (state != m_state)
    {
        std::cout << "SelfParkingCar::StateMachine new state: "
                  << AutoParkECU::StateMachine::to_string(m_state)
                  << std::endl;
    }
}

//-----------------------------------------------------------------------------
AutoParkECU::AutoParkECU(Car& car, std::vector<std::unique_ptr<Car>> const& cars)
  : ECU(), m_ego(car), m_cars(cars)
{
    std::cout << "AutoParkECU created" << std::endl;
}

//-----------------------------------------------------------------------------
void AutoParkECU::update(float const dt)
{
   // std::cout << "AutoParkECU update" << std::endl;
   m_statemachine.update(dt, *this);
}

//-----------------------------------------------------------------------------
// FIXME for the moment a single sensor
// FIXME simulate defectuous sensor
bool AutoParkECU::detect() // FIXME retourner un champ de bit 1 bool par capteur
{
    sf::Vector2f p; // FIXME to be returned

    auto& radars = m_ego.sensors();
    if (radars.size() == 0u)
        return false;

    switch (m_ego.turningIndicator())
    {
        case TurningIndicator::Right:
            assert(radars.size() >= CarBluePrint::WheelName::RR);
            for (auto const& car: m_cars)
            {
                if (radars[CarBluePrint::WheelName::RR]->detects(car->obb(), p))
                    return true;
            }
            break;
        case TurningIndicator::Left:
            assert(radars.size() >= CarBluePrint::WheelName::RL);
            for (auto const& car: m_cars)
            {
                if (radars[CarBluePrint::WheelName::RL]->detects(car->obb(), p))
                    return true;
            }
            break;
        default:
            break;
    }

    return false;
}

//-----------------------------------------------------------------------------
bool AutoParkECU::park(Parking const& parking, bool const entering) // TODO proper
{
   m_trajectory = CarTrajectory::create(parking.type);
   return m_trajectory->init(m_ego, parking, entering);
}

//-----------------------------------------------------------------------------
bool AutoParkECU::updateTrajectory(float const dt)
{
    if (m_trajectory == nullptr)
        return true;

    return false; // FIXME m_trajectory->update(m_control, dt);
}