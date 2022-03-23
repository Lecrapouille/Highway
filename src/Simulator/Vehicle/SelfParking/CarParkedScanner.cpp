// 2021 -- 2022 Quentin Quadrat quentin.quadrat@gmail.com
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <https://unlicense.org>

#include "Vehicle/SelfParking/CarParkedScanner.hpp"

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

    react(s_transitions);
}

// -----------------------------------------------------------------------------
CarParkedScanner::Status CarParkedScanner::update(float dt)
{
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

// -----------------------------------------------------------------------------
const char* CarParkedScanner::stringify(CarParkedScannerStateID const state) const
{
    static const char* s_names[] = {
        [CarParkedScannerStateID::DETECTING_REAR_OF_FIRST_CAR] = "DETECTING_REAR_OF_FIRST_CAR",
        [CarParkedScannerStateID::DETECTING_FRONT_OF_FIRST_CAR] = "DETECTING_FRONT_OF_FIRST_CAR",
        [CarParkedScannerStateID::DETECTING_HOLE] = "DETECTING_HOLE",
        [CarParkedScannerStateID::DETECTING_REAR_OF_SECOND_CAR] = "DETECTING_REAR_OF_SECOND_CAR",
    };

    return s_names[state];
}
