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

//-------------------------------------------------------------------------
void SelfParkingCar::StateMachine::update(float const dt, SelfParkingCar& car)
{
    States state = m_state;

    // User aborted auto-parking
    if (!car.clignotant())
    {
        m_state = States::TRAJECTORY_DONE;
    }

    switch (m_state)
    {
    case States::IDLE:
        if (car.clignotant())
        {
            if (true) // outside the parking spot
            {
                m_state = States::SCAN_PARKING_SPOTS;
                m_scan.start();
            }
            else
            {
                m_state = States::COMPUTE_LEAVING_TRAJECTORY;
            }
        }
        break;

    case States::SCAN_PARKING_SPOTS:
        {
            Scan::Status scanning = m_scan.update(dt, car, m_parking);
            if (scanning == Scan::Status::DETECTED)
            {
                std::cout << "Scan::Status::DETECTED" << std::endl;
                m_state = States::COMPUTE_ENTERING_TRAJECTORY;
            }
            else if (scanning == Scan::Status::NOT_DETECTED)
            {
                std::cout << "Scan::Status::NOT_DETECTED" << std::endl;
                m_state = States::TRAJECTORY_DONE;
            }
            else // scanning == WIP
            {
                // Do nothing
            }
        }
        break;

    case States::COMPUTE_ENTERING_TRAJECTORY:
        car.park(m_parking, true);
        m_state = States::DRIVE_ALONG_TRAJECTORY;
        break;

    case States::COMPUTE_LEAVING_TRAJECTORY:
        car.park(m_parking, false);
        m_state = States::DRIVE_ALONG_TRAJECTORY;
        break;

    case States::DRIVE_ALONG_TRAJECTORY:
        if (!car.hasTrajectory())
        {
            std::cout << "Pas de trajectoire" << std::endl;
            m_state = States::TRAJECTORY_DONE;
        }
        else
        {
            if (car.updateTrajectory(dt) == false)
            {
                std::cout << "trajectoire done" << std::endl;
                m_state = States::TRAJECTORY_DONE;
            }
            else
            {
                // Do nothing: travelling along the trajectory WIP
            }
        }
        break;

    case States::TRAJECTORY_DONE:
        m_state = States::IDLE;
        car.clignotant(false);
        car.setSpeed(0.0f);
        break;
    }

    if (state != m_state)
    {
        std::cout << "SelfParkingCar::StateMachine "
                  << SelfParkingCar::StateMachine::to_string(m_state)
                  << std::endl;
    }
}
