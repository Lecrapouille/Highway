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

#ifndef CAR_PARKED_SCANNER_HPP
#  define CAR_PARKED_SCANNER_HPP

#  include "Common/StateMachine.hpp"
#  include "Vehicle/Car.hpp"

// *****************************************************************************
//! \brief
// *****************************************************************************
enum CarParkedScannerStateID
{
    DETECTING_REAR_OF_FIRST_CAR,
    DETECTING_FRONT_OF_FIRST_CAR,
    DETECTING_HOLE,
    DETECTING_REAR_OF_SECOND_CAR,
    IGNORING_EVENT,
    CANNOT_HAPPEN,
    MAX_STATES
};

// *****************************************************************************
//! \brief The ego car is driving along the road and detect for the first car
//! then detects for a hole large enough for parking then the begining of the
//! second car.
// *****************************************************************************
class CarParkedScanner : public StateMachine<CarParkedScanner, CarParkedScannerStateID>
{
public:

    // ************************************************************************
    //! \brief
    // ************************************************************************
    enum class Status
    {
        PARKING_SLOT_FOUND,
        PARKING_SLOT_NOT_FOUND,
        SCANNING_PARKING_SLOT
    };

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    CarParkedScanner(Car& car);

    //-------------------------------------------------------------------------
    //! \brief Update the state machine.
    //! \return SCANNING_PARKING_SLOT: searching an empty parking slot is in
    //!   progress.
    //! \return PARKING_SLOT_FOUND_PARKING_SLOT: searching an empty parking slot
    //!   ends with success: empty parking slot found.
    //! \return PARKING_SLOT_FOUND_PARKING_SLOT: searching an empty parking slot
    //!   ends without success: empty parking slot was not found.
    //-------------------------------------------------------------------------
    Status update(float dt);

    //-------------------------------------------------------------------------
    //! \brief Returned status of the scanning process.
    //! \return SCANNING_PARKING_SLOT: searching an empty parking slot is in
    //!   progress.
    //! \return PARKING_SLOT_FOUND_PARKING_SLOT: searching an empty parking slot
    //!   ends with success: empty parking slot found.
    //! \return PARKING_SLOT_FOUND_PARKING_SLOT: searching an empty parking slot
    //!   ends without success: empty parking slot was not found.
    //-------------------------------------------------------------------------
    inline Status status() const
    {
        return m_status;
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    void reset();

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    void event_detection(bool detection);

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    void enteringStateDetectingRearOfFirstCar();
    bool guardStateDetectingFrontOfFirstCar();
    void enteringStateDetectingFrontOfFirstCar();
    bool guardStateDetectingHole();
    void enteringStateDetectingHole();
    bool guardStateDetectingRearOfSecondCar();
    void enteringStateDetectingRearOfSecondCar();


private:

    //! \brief The ego car to control
    Car& m_ego;
    //! \brief The estimated distance made by the car since the auto-park has
    //! started.
    float m_distance = 0.0f;
    //! \brief The estimated parking slot length;
    float m_slot_length = 0.0f;
    //! \brief Has the sensor detected something ?
    bool m_detected = false;
    //! \brief Returned status of the scanning process.
    Status m_status = Status::SCANNING_PARKING_SLOT;
};

#endif
