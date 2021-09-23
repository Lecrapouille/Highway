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
SelfParkingCar::Scan::update(float const dt, SelfParkingCar& car, Parking& parking)
{
    States state = m_state;

    if (car.position().x >= 200) // out of parking
    {
        std::cout << "  Outside parking" << std::endl;
        m_state = States::NOT_FOUND;
    }

    switch (m_state)
    {
    case States::NOT_FOUND:
        car.setSpeed(0.0f);
        return Status::NOT_DETECTED;

    case States::IDLE:
        car.setSpeed(10.0f);
        m_distance = 0.0f;
        m_state = States::DETECT_FIRST_CAR;
        return Status::WIP;

    case States::DETECT_FIRST_CAR:
        if (!car.detect())
        {
            std::cout << "DETECT_FIRST_CAR: car.detect false" << std::endl;
            m_position = car.position();
            m_state = States::DETECT_HOLE;
        }
        return Status::WIP;

    case States::DETECT_HOLE:
        if (!car.detect())
        {
            std::cout << "Empty spot distance: " << m_distance << std::endl;
            m_distance += car.speed() * dt;
        }
        else
        {
            std::cout << "DETECT_HOLE: car.detect true" << std::endl;
            m_state = States::DETECT_SECOND_CAR;
        }
        return Status::WIP;

    case States::DETECT_SECOND_CAR:
        if (car.detect())
        {
            std::cout << "DETECT_SECOND_CAR: car.detect true" << std::endl;
            if (m_distance >= 4.8f)
            {
                // TODO know distance 2.0f
                ParkingDimension dim(m_distance, 2.0f, 0u);
                parking = Parking(dim, sf::Vector2f(m_position.x + 0.2f, m_position.y - 2.0f));
                std::cout << parking << std::endl;
                m_state = States::FOUND;
                car.setSpeed(0.0f);
                return Status::DETECTED;
            }
            m_state = States::IDLE;
        }
        return Status::WIP;

    case States::FOUND:
        parking = Parking(ParkingDimensions::get("epi.0"), m_position);
        return Status::DETECTED;
    }

    if (state != m_state)
    {
        std::cout << "  SelfParkingCar::Scan "
                  << SelfParkingCar::Scan::to_string(m_state) << std::endl;
    }

    return Status::NOT_DETECTED;
}
