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
    bool detected = car.detect();

    // Has the driver aborted the auto-parking system ?
    if (!car.turning_left() && !car.turning_right())
    {
        m_state = States::TRAJECTORY_DONE;
    }

    switch (m_state)
    {
    case States::IDLE:
        // Waiting the driver press a button to start the self-parking process.
        if (car.turning_left() || car.turning_right())
        {
            if (true) // TODO car.isParked()
            {
                // The car is not parked: start scanning parked cars
                m_state = States::SCAN_PARKING_SPOTS;
                m_scan.start();
            }
            else
            {
                // The car was parked: compute the path to leave the spot
                m_state = States::COMPUTE_LEAVING_TRAJECTORY;
            }
        }
        break;

    case States::SCAN_PARKING_SPOTS:
        {
            // The car is scanning parked cars to find an empty parking spot
            Scan::Status scanning = m_scan.update(dt, car, detected, m_parking);

            // Empty parking spot detected
            if (scanning == Scan::Status::SUCCEEDED)
            {
                std::cout << "Scan::Status::SUCCEEDED" << std::endl;
                m_state = States::COMPUTE_ENTERING_TRAJECTORY;
            }
            // Failed to find an empty parking spot
            else if (scanning == Scan::Status::FAILED)
            {
                std::cout << "Scan::Status::FAILED" << std::endl;
                m_state = States::TRAJECTORY_DONE;
            }
            else // scanning == Scan::Status::IN_PROGRESS
            {
                // Do nothing: the car is still scanning parked car to find
                // empty parking spot.
            }
        }
        break;

    case States::COMPUTE_ENTERING_TRAJECTORY:
        // Empty parking spot detected: if possible compute a path to the spot
        if (car.park(m_parking, true))
        {
            m_state = States::DRIVE_ALONG_TRAJECTORY;
        }
        else
        {
            m_state = States::IDLE;
        }
        break;

    case States::COMPUTE_LEAVING_TRAJECTORY:
        // Leaving the parking spot detected: if possible compute a path to exit
        // the spot.
        if (car.park(m_parking, false))
        {
            m_state = States::DRIVE_ALONG_TRAJECTORY;
        }
        else
        {
            std::cout << "SORRY I do not know how to leave by myself" << std::endl;
            m_state = States::TRAJECTORY_DONE;
        }
        break;

    case States::DRIVE_ALONG_TRAJECTORY:
        // The car is driving along its computed path
        if (!car.hasTrajectory())
        {
            std::cout << "Pas de trajectoire" << std::endl;
            m_state = States::TRAJECTORY_DONE;
        }
        else if (car.updateTrajectory(dt) == false)
        {
            std::cout << "trajectoire done" << std::endl;
            m_state = States::TRAJECTORY_DONE;
        }
        else
        {
            // Do nothing: the car is currently travelling along the trajectory
        }
        break;

    case States::TRAJECTORY_DONE:
        // Reset the car states
        m_state = States::IDLE;
        car.turning_indicator(false, false);
        car.setRefSpeed(0.0f);
        break;
    }

    // Debug purpose
    if (state != m_state)
    {
        std::cout << "SelfParkingCar::StateMachine "
                  << SelfParkingCar::StateMachine::to_string(m_state)
                  << std::endl;
    }
}
