//=====================================================================
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
//=====================================================================

#ifndef AUTO_PARK_ECU_HPP
#  define AUTO_PARK_ECU_HPP

#  include "Vehicle/ECU.hpp"
#  include "ECUs/AutoParkECU/Trajectories/Trajectory.hpp"
#  include "Sensors/Sensors.hpp"
#  include "Common/Visitor.hpp"
#  include <atomic>
#  include <deque>

class Car;
class Parking;
class City;

// ****************************************************************************
//! \brief ECU controling the ego car for doing autonomous parking maneuvers
// ****************************************************************************
class AutoParkECU : public ECU, public Visitor
{
private:

    // *************************************************************************
    //! \brief Helper class holding the state machine for scaning parked cars
    //! and detect the first empty parking spot. The implementation of this
    //! state machine is made in SelfParkingScanParking.cpp: Detect the first
    //! parked car, then detect the empty spot, then detect the next parked car.
    // *************************************************************************
    class Scanner // FIXME https://github.com/Lecrapouille/Highway/issues/25
    {
        //! \brief Define the differents states of the state machine: Detect the
        //! first parked car, then detect the empty spot, then detect the next
        //! parked car.
        enum States {
            EMPTY_SPOT_NOT_FOUND, IDLE, DETECT_FIRST_CAR, DETECT_EMPTY_SPOT,
            DETECT_SECOND_CAR, EMPTY_SPOT_FOUND
        };

    public:

        //! \brief Return code to the main state machine (the one used for
        //! parking): scanning, parking spot found (success), spot not found
        //! (failure).
        enum Status { IN_PROGRESS, SUCCEEDED, FAILED };

        //----------------------------------------------------------------------
        //! \brief
        //! \param[in] lmin Minimal turning radius for the external point of the
        //! car.
        //----------------------------------------------------------------------
        Scanner(AutoParkECU& ecu, Meter const lmin)
            : m_ecu(ecu), Lmin(lmin)
        {}

        //----------------------------------------------------------------------
        //! \brief Reset the state machine to the initial state.
        //----------------------------------------------------------------------
        inline void start() { m_state = IDLE; }

        //----------------------------------------------------------------------
        //! \brief Update the state machine.
        //! \param[in] dt: delta time in seconds.
        //! \param[inout] car: the reference of the ego car.
        //! \param[in] detection: bitfield of detection returned by sensors.
        //! \return if the parking has found, or not found or still searching an
        //! empty spot.
        //----------------------------------------------------------------------
        Scanner::Status update(Second const dt, Car& car, Antenna::Detection const& detection);

        //----------------------------------------------------------------------
        //! \brief Return detected parking iff Status == SUCCEEDED.
        //----------------------------------------------------------------------
        Parking& parking()
        {
            assert(m_parking != nullptr);
            return *m_parking;
        }

    private:

        //----------------------------------------------------------------------
        //! \brief For debug purpose only.
        //----------------------------------------------------------------------
        std::string to_string(AutoParkECU::Scanner::States s)
        {
            switch (s)
            {
            case AutoParkECU::Scanner::States::EMPTY_SPOT_NOT_FOUND:
                return "EMPTY_SPOT_NOT_FOUND";
            case AutoParkECU::Scanner::States::IDLE:
                return "IDLE";
            case AutoParkECU::Scanner::States::DETECT_FIRST_CAR:
                return "DETECT_FIRST_CAR";
            case AutoParkECU::Scanner::States::DETECT_EMPTY_SPOT:
                return "DETECT_EMPTY_SPOT";
            case AutoParkECU::Scanner::States::DETECT_SECOND_CAR:
                return "DETECT_SECOND_CAR";
            case AutoParkECU::Scanner::States::EMPTY_SPOT_FOUND:
                return "EMPTY_SPOT_FOUND";
            default:
                return "???";
            }
        }

    private:

        //! \brief
        AutoParkECU& m_ecu;
        //! \brief Minimal turning radius for the external point of the car.
        //! FIXME this is computed when doing the parallel trajectory but we also
        //! need here. Can this be factorized ?
        Meter Lmin;
        //! \brief Current state of the state machine.
        AutoParkECU::Scanner::States m_state = AutoParkECU::Scanner::States::IDLE;
        //! \brief Memorize the car position when the first car has been
        //! detected.
        sf::Vector2<Meter> m_position = sf::Vector2<Meter>(0.0_m, 0.0_m);
        //! \brief Estimation of the empty spot length.
        Meter m_spot_length = 0.0_m;
        //! \brief Total vehicle distance since searching for a parking slot
        Meter m_distance = 0.0_m;
        //! \brief Memorize the parking spot once detected.
        std::unique_ptr<Parking> m_parking = nullptr;
    }; // class Scanner

    // *************************************************************************
    //! \brief Main state machine for self-parking: drive along the parking,
    //! scan parked cars and detect empty spot, compute the path for parking and
    //! compute the reference speed and reference steering angle for the cruise
    //! controler.
    // *************************************************************************
    class StateMachine // FIXME https://github.com/Lecrapouille/Highway/issues/25
    {
        //! \brief Define the differents states of the state machine: wait the
        //! event to start parking, scan parked cars, compute the trajectory to
        //! the spot.
        enum States {
            IDLE, SCAN_PARKING_SPOTS, COMPUTE_ENTERING_TRAJECTORY,
            COMPUTE_LEAVING_TRAJECTORY, DRIVE_ALONG_TRAJECTORY,
            TRAJECTORY_DONE
        };

    public:

        //----------------------------------------------------------------------
        //! \brief
        //! \param[in] lmin Minimal turning radius for the external point of the
        //! car.
        //----------------------------------------------------------------------
        StateMachine(AutoParkECU& ecu, Meter const lmin)
            : m_ecu(ecu), m_scanner(ecu, lmin)
        {}

        //----------------------------------------------------------------------
        //! \brief Update the state machine.
        //! \param[in] dt: delta time in seconds.
        //! \param[inout] ecu: the reference of the ego car's ECU.
        //----------------------------------------------------------------------
        void update(Second const dt, AutoParkECU& ecu);

    private:

        //----------------------------------------------------------------------
        //! \brief For debug purpose only.
        //----------------------------------------------------------------------
        std::string to_string(StateMachine::States s)
        {
            switch (s)
            {
            case AutoParkECU::StateMachine::States::IDLE:
                return "IDLE";
            case AutoParkECU::StateMachine::States::SCAN_PARKING_SPOTS:
                return "SCAN_PARKING_SPOTS";
            case AutoParkECU::StateMachine::States::COMPUTE_ENTERING_TRAJECTORY:
                return "COMPUTE_ENTERING_TRAJECTORY";
            case AutoParkECU::StateMachine::States::COMPUTE_LEAVING_TRAJECTORY:
                return "COMPUTE_LEAVING_TRAJECTORY";
            case AutoParkECU::StateMachine::States::DRIVE_ALONG_TRAJECTORY:
                return "DRIVE_ALONG_TRAJECTORY";
            case AutoParkECU::StateMachine::States::TRAJECTORY_DONE:
                return "TRAJECTORY_DONE";
            default:
                return "???";
            }
        }

        //! \brief
        AutoParkECU& m_ecu;
        //! \brief Current state of the state machine.
        States m_state = States::IDLE;
        //! \brief Parking spot scanner state machine.
        AutoParkECU::Scanner m_scanner;
    }; // class StateMachine

public:

   AutoParkECU(Car& car, City const& city);
   COMPONENT_CLASSTYPE(AutoParkECU, ECU);

public:

   virtual void update(Second const dt) override;

    //-------------------------------------------------------------------------
    //! \brief Return true if the car has a trajectory and this case trajectory()
    //! can be used.
    //-------------------------------------------------------------------------
    bool hasTrajectory() const
    {
        return m_trajectory != nullptr;
    }

    //-------------------------------------------------------------------------
    //! \brief Const getter of the trajectory if and only if hasTrajectory()
    //! return true.
    //-------------------------------------------------------------------------
    CarTrajectory const& trajectory() const
    {
        assert(m_trajectory != nullptr);
        return *m_trajectory;
    }

private: // Inheritance

    //-------------------------------------------------------------------------
    //! \brief Visitor pattern. Do concreate action on the given sensor.
    //-------------------------------------------------------------------------
    virtual void operator()(Antenna& antenna) override;

    //-------------------------------------------------------------------------
    //! \brief Observer pattern. Do concreate action on the given sensor when
    //! it has been updated.
    //-------------------------------------------------------------------------
    virtual void onSensorUpdated(Sensor& sensor) override;

private:

    //-------------------------------------------------------------------------
    //! \brief Check if the car detects other car.
    //! \return true if a car has been detected by one of sensors.
    //-------------------------------------------------------------------------
    Antenna::Detection const& detect();

    //-------------------------------------------------------------------------
    //! \brief Create the trajectory to the given parking spot.
    //! \param[in] parking: the parking spot to park in.
    //! \return true if the car can park on it, else return false.
    //-------------------------------------------------------------------------
    bool park(Parking const& parking, bool const entering);

    //-------------------------------------------------------------------------
    //! \brief Update the trajectory (get current references and make it use
    //! by cruise controller).
    //-------------------------------------------------------------------------
    bool updateTrajectory(Second const dt);

private:

    //! \brief The vehicle owning this ECU for entering in the parking slot.
    Car& m_ego;
    //! \brief To access to list of objects collidable with this ego vehicle.
    City const& m_city;
    //! \brief Main state machine for searching and entering in the first parking
    //! slot. See doc/ParkingStateMachine.jpg
    StateMachine m_statemachine;
    //! \brief If and only if reachable, the trajectory to the parking slot.
    std::unique_ptr<CarTrajectory> m_trajectory = nullptr;
    //! \brief Trigger for starting searching the first parking slot and
    //! computing the trajectory to enter in.
    std::atomic<bool> m_clignotant{false};
    //! \brief Has sensor detect a parked vehicle ? Default value is set to invalid
    Antenna::Detection m_detection;
};

#endif
