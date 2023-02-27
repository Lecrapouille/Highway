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
#include "ECUs/AutoParkECU/ParallelTrajectory.hpp"
#include "Vehicle/TurningRadius.hpp"
#include "Vehicle/Car.hpp"
#include "City/Parking.hpp"
#include "Simulation/BluePrints.hpp"
#include <iostream>

// FIXME https://github.com/Lecrapouille/Highway/issues/14
// How to access to MessageBar to remove std::cout << ?

//------------------------------------------------------------------------------
// doc/StateMachines/ScanStateMachine.jpg
AutoParkECU::Scanner::Status
AutoParkECU::Scanner::update(Second const dt, Car& car,
                             Antenna::Detection const& detection)
{
    States state = m_state;

    // Estimate the distance made by the vehicle from its speed information.
    m_distance += car.speed() * dt;

    // Disable the auto-park ECU if the car travelled too many distance from its
    // initial position to find an empty parking spot.
    if (m_distance >= 12.0_m)
    {
        m_ecu.logMessage("Max distance reached: could not found parking slot");
        m_state = AutoParkECU::Scanner::States::EMPTY_SPOT_NOT_FOUND;
    }

    switch (m_state)
    {
    case AutoParkECU::Scanner::States::EMPTY_SPOT_NOT_FOUND:
        // The car did not found the parking spot: reset states.
        return AutoParkECU::Scanner::Status::FAILED;

    case AutoParkECU::Scanner::States::IDLE:
        // The car was stopped and now it has to drive along the parking spots
        // and to scan for parked cars in the aim to find the first empty
        // parking spot.
        m_spot_length = 0.0_m;
        m_distance = 0.0_m;
        m_state = AutoParkECU::Scanner::States::DETECT_FIRST_CAR;
        return AutoParkECU::Scanner::Status::IN_PROGRESS;

    case AutoParkECU::Scanner::States::DETECT_FIRST_CAR:
        // The car has detected the first parked car. Now we have to estimate
        // the gap length with the next parked car. Depending on the gap length
        // we can deduce we have found a real parking spot or either the space
        // between parked cars.
        if (!detection.valid)
        {
            m_position = car.position();
            m_state = AutoParkECU::Scanner::States::DETECT_EMPTY_SPOT;
        }
        return AutoParkECU::Scanner::Status::IN_PROGRESS;

    case AutoParkECU::Scanner::States::DETECT_EMPTY_SPOT:
        // The car is detecting a gap between parked cars but is it a real
        // parking spot? To confirm it, we have to integrate the vehicle speed
        // to know the length of the gap.
        if (detection.valid)
        {
            m_state = AutoParkECU::Scanner::States::DETECT_SECOND_CAR;
        }
        else if (m_spot_length >= Lmin)
        {
            // If the gap between vehicle is enough large for doing a
            // single-trial trajectory (i.e. two consecutive empty spots) avoid
            // to drive up to the next parked car but let do the maneuver
            // directly.
            m_state = AutoParkECU::Scanner::States::DETECT_SECOND_CAR;
        }

        // Estimate the parking spot length by integrating the vehicle
        // longitunidal speed.
        m_spot_length += car.speed() * dt;
        return AutoParkECU::Scanner::Status::IN_PROGRESS;

    case AutoParkECU::Scanner::States::DETECT_SECOND_CAR:
        if (detection.valid && (m_spot_length <= car.blueprint.length))
        {
            // The gap between parked cars is too small: this is not a parking,
            // so let continuing scanning other parked cars.
             m_ecu.logMessage("Scan: No way to park at X: ",
                              m_position.x,
                              " because distance is too short (",
                              m_spot_length, ")");
            m_state = AutoParkECU::Scanner::States::DETECT_FIRST_CAR;
        }
        else if (detection.valid || m_spot_length >= Lmin)
        {
            // TODO Missing detection of the type of parking type.
            // https://github.com/Lecrapouille/Highway/issues/32
            Meter pw = BluePrints::get<ParkingBluePrint>("epi.0").width;
            ParkingBluePrint dim(m_spot_length, pw, 0.0_deg);
            //ParkingBluePrint dim = BluePrints::get<ParkingBluePrint>("epi.0");
            m_parking = std::make_unique<Parking>
                        (dim, sf::Vector2<Meter>(//107.0_m, 101.0_m),
                         m_position.x, m_position.y /*- detection.distance*/ - 0.5 * pw),
                         0.0_deg); // FIXME matrice de passage entre coordonnees monde et coordonnees du parking
            m_ecu.logMessage("Scan: Parking spot detected: ", *m_parking);
            m_state = AutoParkECU::Scanner::States::EMPTY_SPOT_FOUND;
            return AutoParkECU::Scanner::Status::SUCCEEDED;
        }
        m_state = AutoParkECU::Scanner::States::IDLE;
        return AutoParkECU::Scanner::Status::IN_PROGRESS;

    case AutoParkECU::Scanner::States::EMPTY_SPOT_FOUND:
        // The parking was found, stay in this state and let returning the
        // currently found parking dimension.
        return AutoParkECU::Scanner::Status::SUCCEEDED;
    }

    // Debug purpose
    if (state != m_state)
    {
        auto const& s = AutoParkECU::Scanner::to_string(m_state);
        m_ecu.logMessage("SelfParkingCar::Scan new state: ", s);
    }

    return AutoParkECU::Scanner::Status::FAILED;
}

//------------------------------------------------------------------------------
// doc/StateMachines/ParkingStateMachine.jpg
void AutoParkECU::StateMachine::update(Second const dt, AutoParkECU& ecu)
{
    States state = m_state;

    // Has the driver aborted the auto-parking system ?
    if ((m_state != AutoParkECU::StateMachine::States::IDLE) &&
        (ecu.m_ego.turningIndicator.state() == TurningIndicator::Off))
    {
        m_ecu.logMessage("The driver has aborted the auto-parking");
        m_state = AutoParkECU::StateMachine::States::TRAJECTORY_DONE;
    }

    switch (m_state)
    {
    case AutoParkECU::StateMachine::States::IDLE:
        // Waiting the driver has pressed the turning indicator to start the
        // self-parking process.
        if ((ecu.m_ego.turningIndicator.state() == TurningIndicator::Left) ||
            (ecu.m_ego.turningIndicator.state() == TurningIndicator::Right))
        {
            if (true) // TODO car.isParked() https://github.com/Lecrapouille/Highway/issues/28
            {
                // The car was not parked when the self-parking process has
                // started: let scan parked cars along the road finding the
                // first empty parking spot.
                ecu.m_ego.showSensors(true);
                m_state = AutoParkECU::StateMachine::States::SCAN_PARKING_SPOTS;
                m_scanner.start();
                ecu.m_ego.refSteering(0.0_deg);
                ecu.m_ego.refSpeed(2.0_mps);
            }
            else
            {
                // The car was parked when the self-parking process has started:
                // let compute the path to leave the spot.
                m_state = AutoParkECU::StateMachine::States::COMPUTE_LEAVING_TRAJECTORY;
            }
        }
        break;

    case AutoParkECU::StateMachine::States::SCAN_PARKING_SPOTS:
        {
            // The car is scanning parked cars to find an empty parking spot.
            Antenna::Detection const& detection = ecu.detect();
            AutoParkECU::Scanner::Status scanning = m_scanner.update(dt, ecu.m_ego, detection);

            // Empty parking spot detected.
            if (scanning == AutoParkECU::Scanner::Status::SUCCEEDED)
            {
                ecu.m_ego.refSpeed(0.0_mps);
                m_state = AutoParkECU::StateMachine::States::COMPUTE_ENTERING_TRAJECTORY;
            }
            // Failed to find an empty parking spot.
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
            m_ecu.logMessage("SORRY I do not know how to leave"
                             "by myself.Not yet implemented");
            m_state = AutoParkECU::StateMachine::States::TRAJECTORY_DONE;
        }
        break;

    case AutoParkECU::StateMachine::States::DRIVE_ALONG_TRAJECTORY:
        // The car is driving along its computed path to the parking spot.
        if (!ecu.hasTrajectory())
        {
            m_ecu.logMessage("No trajectory found");
            m_state = AutoParkECU::StateMachine::States::TRAJECTORY_DONE;
        }
        else if (ecu.updateTrajectory(dt) == false)
        {
            m_ecu.logMessage("Trajectory done");
            m_state = AutoParkECU::StateMachine::States::TRAJECTORY_DONE;
        }
        else
        {
            // Do nothing: the car is currently driving along the trajectory to
            // the parking spot.
        }
        break;

    case AutoParkECU::StateMachine::States::TRAJECTORY_DONE:
        // Reset the car states
        m_state = AutoParkECU::StateMachine::States::IDLE;
        ecu.m_ego.turningIndicator.state(TurningIndicator::Off);
        ecu.m_ego.refSpeed(0.0_mps);
        ecu.m_ego.showSensors(false);
        break;
    }

    // Debug purpose
    if (state != m_state)
    {
        auto const& s = AutoParkECU::StateMachine::to_string(m_state);
        m_ecu.logMessage("SelfParkingCar::StateMachine new state: ", s);
    }
}

//------------------------------------------------------------------------------
// doc/Parallel/ParallelLeavingCondition.png
static Meter computeLmin(Car const& car)
{
    TurningRadius radius(car.blueprint, car.blueprint.max_steering_angle);
    return car.blueprint.back_overhang + units::math::sqrt(
        radius.external * radius.external - radius.internal * radius.internal);
}

//------------------------------------------------------------------------------
AutoParkECU::AutoParkECU(Car& car, City const& city)
    : m_ego(car), m_city(city), m_statemachine(*this, computeLmin(car))
{
    LOGI("AutoParkECU created");
}

//------------------------------------------------------------------------------
void AutoParkECU::update(Second const dt)
{
    // doc/StateMachines/ParkingStrategyFig.png
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
    // Detect parked vehicles on the side given by the turning indicator.
    switch (m_ego.turningIndicator.state())
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

    // Has detected a parked vehicle ?
    if (antenna.detection().valid)
    {
        m_detection = antenna.detection();
        m_detection.distance = 0.2_m; // FIXME
        // std::cout << "******* DETECTION " << m_detection.distance << " P: "
        // << m_detection.position.x << ", " << m_detection.position.x << std::endl;

        antenna.shape.color = sf::Color::Red;
    }
    else
    {
        antenna.shape.color = sf::Color::Green;
    }
}

//------------------------------------------------------------------------------
Antenna::Detection const& AutoParkECU::detect()
{
    m_detection.valid = false;
    for (auto const& sensor: m_ego.sensors())
    {
        // This will call AutoParkECU::operator()(X&) with X of the correct type
        // (Antenna i.e.).
        sensor->accept(*this);
    }

    return m_detection;
}

//------------------------------------------------------------------------------
bool AutoParkECU::park(Parking const& parking, bool const entering)
{
    std::cout << "CCCCCCCCCCCCCCCCCCC\n";
    // Parallel, perpendicular, diagonal trajectories.
    m_trajectory = CarTrajectory::create(*this, parking.type);
    return m_trajectory->init(m_ego, parking, entering);
}

//------------------------------------------------------------------------------
bool AutoParkECU::updateTrajectory(Second const dt)
{
    // The vehicle cannot park or is already parked or has lelft.
    if (m_trajectory == nullptr)
        return false;

    // In the case of parallel trajectory.
    // doc/Parallel/ParallelFinalStep.png
    if (m_trajectory->update(m_ego, dt))
        return true;

    // End of the trajectory, delete it since we no longer need it
    // (this will avoid rendering it i.e.)
    m_trajectory = nullptr;
    return false;
}
