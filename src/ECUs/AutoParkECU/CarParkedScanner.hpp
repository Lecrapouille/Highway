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
