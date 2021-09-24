// 2021 Quentin Quadrat quentin.quadrat@gmail.com
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

#include "SelfParking/SelfParkingVehicle.hpp"
#include "World/Parking.hpp"
#include "World/Blueprints.hpp"

//-------------------------------------------------------------------------
SelfParkingCar::Scan::Status
SelfParkingCar::Scan::update(float const dt, SelfParkingCar& car, bool detected, Parking& parking)
{
    States state = m_state;

    // This condition is purely for the simulation: is the ego car outside the
    // simulation game (parking area) ?
    if (car.position().x >= 140.0f) // out of parking
    {
        std::cout << "  Outside parking" << std::endl;
        m_state = States::EMPTY_SPOT_NOT_FOUND;
    }

    switch (m_state)
    {
    case States::EMPTY_SPOT_NOT_FOUND:
        // The car did not found the parking spot: reset states.
        car.setRefSpeed(0.0f);
        return Status::FAILED;

    case States::IDLE:
        // The car was stopped and now it has to drive along parking spots and
        // scan parked cars to find the first empty parking spot.
        car.setRefSpeed(3.0f);
        m_distance = 0.0f;
        m_state = States::DETECT_FIRST_CAR;
        return Status::IN_PROGRESS;

    case States::DETECT_FIRST_CAR:
        // The car has detected the first parked car. Now search a gap (either
        // space between car either real parking spot).
        if (!detected) // FIXME bitfield
        {
            std::cout << "DETECT_FIRST_CAR: car.detect false" << std::endl;
            m_position = car.position();
            m_state = States::DETECT_EMPTY_SPOT;
        }
        return Status::IN_PROGRESS;

    case States::DETECT_EMPTY_SPOT:
        // The car is detecting a "hole". Is it a real parking spot ? To know it
        // it integrates its speed to know the length of the spot.
        if (detected)
        {
            std::cout << "DETECT_EMPTY_SPOT: car.detect true" << std::endl;
            m_state = States::DETECT_SECOND_CAR;
        }
        else if (m_distance >= 6.4f) // meters FIXME should be Lmin
        {
            // two consecutive empty spots: avoid to drive to the next parked
            // car do the maneuver directly
            std::cout << "DETECT_EMPTY_SPOT: car.detect true" << std::endl;
            m_state = States::DETECT_SECOND_CAR;
        }
        else
        {
            std::cout << "Empty spot distance: " << m_distance << std::endl;
            m_distance += car.speed() * dt;
        }
        return Status::IN_PROGRESS;

    case States::DETECT_SECOND_CAR:
        if (detected && (m_distance <= car.dim.length))
        {
            // Too small length: continue scanning parked cars
            std::cout << "No way to park here!!!" << std::endl;
            m_state = States::DETECT_FIRST_CAR;
        }
        else if (detected || m_distance >= 6.4f) // FIXME should be Lmin
        {
            car.setRefSpeed(0.0f);

            // TODO Missing detection of the type of parking type
            ParkingDimension dim(m_distance, 2.0f, 0u);
            parking = Parking(dim, sf::Vector2f(m_position.x + 0.2f, m_position.y - 2.0f));
            m_parking = Parking(dim, sf::Vector2f(m_position.x + 0.2f, m_position.y - 2.0f));
            std::cout << "Parking detected: " << parking << std::endl;
            m_state = States::EMPTY_SPOT_FOUND;
            return Status::SUCCEEDED;
        }
        m_state = States::IDLE;
        return Status::IN_PROGRESS;

    case States::EMPTY_SPOT_FOUND:
        // The parking was found, stay in this state and return the parking
        car.setRefSpeed(0.0f);
        parking = m_parking;
        return Status::SUCCEEDED;
    }

    // Debug purpose
    if (state != m_state)
    {
        std::cout << "  SelfParkingCar::Scan "
                  << SelfParkingCar::Scan::to_string(m_state) << std::endl;
    }

    return Status::FAILED;
}
