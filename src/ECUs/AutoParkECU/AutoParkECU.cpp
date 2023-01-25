//==============================================================================
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
//==============================================================================

#include "ECUs/AutoParkECU/AutoParkECU.hpp"
#include "ECUs/AutoParkECU/Trajectories/ParallelTrajectory.hpp"
#include "Simulator/Vehicle/TurningRadius.hpp"
#include "Simulator/Vehicle/Car.hpp"
#include "Simulator/City/Parking.hpp"
#include "Simulator/BluePrints.hpp"
#include <iostream>

// FIXME https://github.com/Lecrapouille/Highway/issues/14
// How to access to MessageBar to remove std::cout << ?

//------------------------------------------------------------------------------
AutoParkECU::Scanner::Status
AutoParkECU::Scanner::update(Second const dt, Car& car, Antenna::Detection const& detection)
{
    States state = m_state;

    // This condition is purely for the simulation: is the ego car outside the
    // simulation game (parking area) ?
    m_distance += car.speed() * dt;
    if (m_distance >= 30.0_m)
    {
        std::cout << "  Max distance reached: coukd not found parking slot" << std::endl;
        m_state = AutoParkECU::Scanner::States::EMPTY_SPOT_NOT_FOUND;
    }

    switch (m_state)
    {
    case AutoParkECU::Scanner::States::EMPTY_SPOT_NOT_FOUND:
        // The car did not found the parking spot: reset states.
        return AutoParkECU::Scanner::Status::FAILED;

    case AutoParkECU::Scanner::States::IDLE:
        // The car was stopped and now it has to drive along parking spots and
        // scan parked cars to find the first empty parking spot.
        m_slot_length = 0.0_m;
        m_distance = 0.0_m;
        m_state = AutoParkECU::Scanner::States::DETECT_FIRST_CAR;
        return AutoParkECU::Scanner::Status::IN_PROGRESS;

    case AutoParkECU::Scanner::States::DETECT_FIRST_CAR:
        // The car has detected the first parked car. Now search a gap (either
        // space between car either real parking spot).
        if (!detection.valid)
        {
            m_position = car.position();
            m_state = AutoParkECU::Scanner::States::DETECT_EMPTY_SPOT;
        }
        return AutoParkECU::Scanner::Status::IN_PROGRESS;

    case AutoParkECU::Scanner::States::DETECT_EMPTY_SPOT:
        // The car is detecting a "hole". Is it a real parking spot ? To know it
        // it integrates its speed to know the length of the spot.
        if (detection.valid)
        {
            m_state = AutoParkECU::Scanner::States::DETECT_SECOND_CAR;
        }
        else if (m_slot_length >= Lmin)
        {
            // two consecutive empty spots: avoid to drive to the next parked
            // car do the maneuver directly
            m_state = AutoParkECU::Scanner::States::DETECT_SECOND_CAR;
        }

        m_slot_length += car.speed() * dt;
        return AutoParkECU::Scanner::Status::IN_PROGRESS;

    case AutoParkECU::Scanner::States::DETECT_SECOND_CAR:
        if (detection.valid && (m_slot_length <= car.blueprint.length))
        {
            // Too small length: continue scanning parked cars
            std::cout << "Scan: No way to park at X: " << m_position.x
                      << " because distance is too short (" << m_slot_length << " m)" << std::endl;
            m_state = AutoParkECU::Scanner::States::DETECT_FIRST_CAR;
        }
        else if (detection.valid || m_slot_length >= Lmin)
        {
            // TODO Missing detection of the type of parking type.
            // https://github.com/Lecrapouille/Highway/issues/32
            Meter pw = BluePrints::get<ParkingBluePrint>("epi.0").width;
            ParkingBluePrint dim(m_slot_length, pw, 0.0_deg);
            m_parking = std::make_unique<Parking>(dim, sf::Vector2<Meter>(m_position.x, m_position.y - detection.distance), 0.0_deg);
            std::cout << "Scan: Parking spot detected: " << *m_parking << std::endl;
            m_state = AutoParkECU::Scanner::States::EMPTY_SPOT_FOUND;
            return AutoParkECU::Scanner::Status::SUCCEEDED;
        }
        m_state = AutoParkECU::Scanner::States::IDLE;
        return AutoParkECU::Scanner::Status::IN_PROGRESS;

    case AutoParkECU::Scanner::States::EMPTY_SPOT_FOUND:
        // The parking was found, stay in this state and return the parking
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

//------------------------------------------------------------------------------
void AutoParkECU::StateMachine::update(Second const dt, AutoParkECU& ecu)
{
    States state = m_state;

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
            if (true) // TODO car.isParked() https://github.com/Lecrapouille/Highway/issues/28
            {
                // The car is not parked: start scanning parked cars
                ecu.m_ego.showSensors(true);
                m_state = AutoParkECU::StateMachine::States::SCAN_PARKING_SPOTS;
                m_scanner.start();
                ecu.m_ego.refSteering(0.0_deg);
                ecu.m_ego.refSpeed(2.0_mps);
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
            Antenna::Detection const& detection = ecu.detect();
            AutoParkECU::Scanner::Status scanning = m_scanner.update(dt, ecu.m_ego, detection);

            // Empty parking spot detected
            if (scanning == AutoParkECU::Scanner::Status::SUCCEEDED)
            {
                ecu.m_ego.refSpeed(0.0_mps);
                m_state = AutoParkECU::StateMachine::States::COMPUTE_ENTERING_TRAJECTORY;
            }
            // Failed to find an empty parking spot
            else if (scanning == AutoParkECU::Scanner::Status::FAILED)
            {
                ecu.m_ego.refSpeed(0.0_mps);
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
            // FIXME https://github.com/Lecrapouille/Highway/issues/29
            std::cerr << "SORRY I do not know how to leave by myself. Not yet implemented" << std::endl;
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
        ecu.m_ego.refSpeed(0.0_mps);
        ecu.m_ego.showSensors(false);
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

//------------------------------------------------------------------------------
static Meter computeLmin(Car const& car)
{
    TurningRadius radius(car.blueprint, car.blueprint.max_steering_angle);
    return car.blueprint.back_overhang + units::math::sqrt(
        radius.external * radius.external - radius.internal * radius.internal);
}

//------------------------------------------------------------------------------
AutoParkECU::AutoParkECU(Car& car, City const& city)
    : m_ego(car), m_city(city), m_statemachine(computeLmin(car))
{
    LOGI("AutoParkECU created");
}

//------------------------------------------------------------------------------
void AutoParkECU::update(Second const dt)
{
    m_statemachine.update(dt, *this);
}

//------------------------------------------------------------------------------
void AutoParkECU::onSensorUpdated(Sensor& sensor)
{
    // Make the visitor pattern dispatch
    sensor.accept(*this);
}

//------------------------------------------------------------------------------
void AutoParkECU::operator()(Antenna& antenna)
{
    switch (m_ego.turningIndicator())
    {
        case TurningIndicator::Right:
            if (antenna.name != "antenna_RR")
                return ;
            break;
        case TurningIndicator::Left:
            if (antenna.name != "antenna_RL")
                return ;
            break;
        default:
            antenna.shape.color = antenna.shape.initial_color;
            return;
    }

    if (antenna.detection().valid)
    {
        m_detection = antenna.detection();
        m_detection.distance = 5.0_m;
        //std::cout << "******* DETECTION " << m_detection.distance << " P: " << m_detection.position.x << ", " << m_detection.position.x << std::endl;

        antenna.shape.color = sf::Color::Red;
    }
    else
    {
        antenna.shape.color = sf::Color::Green;
    }
}

//------------------------------------------------------------------------------
// https://github.com/Lecrapouille/Highway/issues/30
// FIXME for the moment a single sensor is used
// FIXME simulate defectuous sensor
Antenna::Detection const& AutoParkECU::detect()
{
    m_detection.valid = false;
    for (auto const& sensor: m_ego.sensors())
    {
        // This will call AutoParkECU::operator()(XXX&)
        sensor->accept(*this);
    }

    return m_detection;
}

//------------------------------------------------------------------------------
bool AutoParkECU::park(Parking const& parking, bool const entering)
{
    m_trajectory = CarTrajectory::create(parking.type);
    return m_trajectory->init(m_ego, parking, entering);
}

//------------------------------------------------------------------------------
bool AutoParkECU::updateTrajectory(Second const dt)
{
    if (m_trajectory == nullptr)
        return true;

    if (m_trajectory->update(m_ego, dt))
        return true;

    // End of the trajectory, delete it since we no longer need it
    // (this will avoid rendering it i.e.)
    m_trajectory = nullptr;
    return false;
}
