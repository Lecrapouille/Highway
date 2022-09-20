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

#include "ECUs/AutoParkECU/CarParkedScanner.hpp"

// -----------------------------------------------------------------------------
const char* stringify(CarParkedScannerStateID const state)
{
    static const char* s_names[] = {
        [CarParkedScannerStateID::DETECTING_REAR_OF_FIRST_CAR] = "DETECTING_REAR_OF_FIRST_CAR",
        [CarParkedScannerStateID::DETECTING_FRONT_OF_FIRST_CAR] = "DETECTING_FRONT_OF_FIRST_CAR",
        [CarParkedScannerStateID::DETECTING_HOLE] = "DETECTING_HOLE",
        [CarParkedScannerStateID::DETECTING_REAR_OF_SECOND_CAR] = "DETECTING_REAR_OF_SECOND_CAR",
    };

    return s_names[state];
}

// -----------------------------------------------------------------------------
void CarParkedScanner::enteringStateDetectingRearOfFirstCar()
{
    std::cout << "Entering in state DetectingRearOfFirstCar" << std::endl;

    // The car was stopped and now it has to drive along parking spots and
    // scan parked cars to find the first empty parking spot.
    m_ego.refSpeed(2.0f);
    m_ego.refSteering(0.0f);
    m_distance = 0.0f;
    //m_detected = false;
    m_status = CarParkedScanner::Status::SCANNING_PARKING_SLOT;
}

// -----------------------------------------------------------------------------
bool CarParkedScanner::guardStateDetectingFrontOfFirstCar()
{
    std::cout << "Guard state DetectingFrontOfFirstCar" << std::endl;
    return !m_detected;
}

// -----------------------------------------------------------------------------
void CarParkedScanner::enteringStateDetectingFrontOfFirstCar()
{
    std::cout << "Entering in state DetectingFrontOfFirstCar" << std::endl;
    //m_position = m_ego.position();
}

// -----------------------------------------------------------------------------
bool CarParkedScanner::guardStateDetectingHole()
{
    std::cout << "Guard state DetectingHole" << std::endl;
    return m_detected;
}

// -----------------------------------------------------------------------------
void CarParkedScanner::enteringStateDetectingHole()
{
    std::cout << "Entering in state DetectingHole" << std::endl;
    m_slot_length = 0.0f;
}

// -----------------------------------------------------------------------------
bool CarParkedScanner::guardStateDetectingRearOfSecondCar()
{
    std::cout << "Guard state DetectingRearOfSecondCar" << std::endl;
    return m_detected || m_slot_length >= 6.4f;
}

// -----------------------------------------------------------------------------
void CarParkedScanner::enteringStateDetectingRearOfSecondCar()
{
    std::cout << "Entering in state DetectingRearOfSecondCar" << std::endl;
    m_ego.refSpeed(0.0f);
}

// -----------------------------------------------------------------------------
CarParkedScanner::CarParkedScanner(Car& car)
    : StateMachine(CarParkedScannerStateID::DETECTING_REAR_OF_FIRST_CAR),
      m_ego(car)
{
    m_states[CarParkedScannerStateID::DETECTING_REAR_OF_FIRST_CAR] = {
        .entering = &CarParkedScanner::enteringStateDetectingRearOfFirstCar,
    };

    m_states[CarParkedScannerStateID::DETECTING_FRONT_OF_FIRST_CAR] = {
        .guard = &CarParkedScanner::guardStateDetectingFrontOfFirstCar,
        .entering = &CarParkedScanner::enteringStateDetectingFrontOfFirstCar,
    };

    m_states[CarParkedScannerStateID::DETECTING_HOLE] = {
        .guard = &CarParkedScanner::guardStateDetectingHole,
        .entering = &CarParkedScanner::enteringStateDetectingHole,
    };

    m_states[CarParkedScannerStateID::DETECTING_REAR_OF_SECOND_CAR] = {
        .guard = &CarParkedScanner::guardStateDetectingRearOfSecondCar,
        .entering = &CarParkedScanner::enteringStateDetectingRearOfSecondCar,
    };
}

// -----------------------------------------------------------------------------
void CarParkedScanner::reset()
{
    StateMachine<CarParkedScanner, CarParkedScannerStateID>::reset();
    m_status = Status::PARKING_SLOT_NOT_FOUND;
}

// -----------------------------------------------------------------------------
void CarParkedScanner::event_detection(bool detection)
{
    m_detected = detection;

    static Transitions s_transitions = {
        {
            CarParkedScannerStateID::DETECTING_REAR_OF_FIRST_CAR,
            CarParkedScannerStateID::DETECTING_FRONT_OF_FIRST_CAR
        },
        {
            CarParkedScannerStateID::DETECTING_FRONT_OF_FIRST_CAR,
            CarParkedScannerStateID::DETECTING_HOLE
        },
        {
            CarParkedScannerStateID::DETECTING_HOLE,
            CarParkedScannerStateID::DETECTING_REAR_OF_SECOND_CAR
        },
    };

    transition(s_transitions);
}

// -----------------------------------------------------------------------------
CarParkedScanner::Status CarParkedScanner::update(float dt)
{
    std::cout << "CarParkedScanner update" << std::endl;

    // Estimate the travelled distance and abort if the car made too long travel
    m_distance += m_ego.speed() * dt;
    if (m_distance >= 25.0f)
    {
        reset();
        return m_status;
    }

    // Estimate the length of the parking slot.
    if (state() == CarParkedScannerStateID::DETECTING_HOLE)
    {
        m_slot_length += m_ego.speed() * dt;
    }

    // Sensor detecting parked car.
    //FIXME  event_detection(m_ego.sensor.detect());

    // Empty parking slot detected or work in progress or failed finding one
    return m_status;
}
