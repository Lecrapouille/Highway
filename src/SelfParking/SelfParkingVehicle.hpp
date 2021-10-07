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

#ifndef SELF_PARKING_VEHICLE_HPP
#  define SELF_PARKING_VEHICLE_HPP

#  include "Vehicle/Vehicle.hpp"
#  include "SelfParking/Trajectories/TurningRadius.hpp"
#  include "SelfParking/Trajectories/Trajectory.hpp"

// ****************************************************************************
//! \brief Vehicle class with Self-Parking system.
//! ../../doc/design/car.png
// ****************************************************************************
class SelfParkingCar: public Car
{
private:

    // *************************************************************************
    //! \brief Helper class holding the state machine for scaning parked cars
    //! and detect the first empty parking spot. The implementation of this
    //! state machine is made in SelfParkingScanParking.cpp: Detect the first
    //! parked car, then detect the empty spot, then detect the next parked car.
    // *************************************************************************
    class Scan
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
        //! \brief Default constructor. m_parking is initialized with dummy
        //! value.
        //----------------------------------------------------------------------
        Scan()
            : m_parking(ParkingDimensions::get("epi.45"), sf::Vector2f(0.0f, 0.0f))
        {}

        //----------------------------------------------------------------------
        //! \brief Reset the state machine to the initial state.
        //----------------------------------------------------------------------
        inline void start() { m_state = IDLE; }

        //----------------------------------------------------------------------
        //! \brief Update the state machine.
        //! \param[in] dt: delta time in seconds.
        //! \param[inout] car: the reference of the ego car.
        //! \param[in] detected: bitfield of detection returned by sensors.
        //! \param[out] parking: return the parking dimension if and if
        //! SUCCEEDED is returned (else the content is unchanged).
        //! \return if the parking has found, or not found or still searching an
        //! empty spot.
        //----------------------------------------------------------------------
        Scan::Status update(float const dt, SelfParkingCar& car, bool detected, Parking& parking);

    private:

        //----------------------------------------------------------------------
        //! \brief For debug purpose only.
        //----------------------------------------------------------------------
        std::string to_string(SelfParkingCar::Scan::States s)
        {
            switch (s)
            {
            case SelfParkingCar::Scan::States::EMPTY_SPOT_NOT_FOUND:
                return "EMPTY_SPOT_NOT_FOUND";
            case SelfParkingCar::Scan::States::IDLE:
                return "IDLE";
            case SelfParkingCar::Scan::States::DETECT_FIRST_CAR:
                return "DETECT_FIRST_CAR";
            case SelfParkingCar::Scan::States::DETECT_EMPTY_SPOT:
                return "DETECT_EMPTY_SPOT";
            case SelfParkingCar::Scan::States::DETECT_SECOND_CAR:
                return "DETECT_SECOND_CAR";
            case SelfParkingCar::Scan::States::EMPTY_SPOT_FOUND:
                return "EMPTY_SPOT_FOUND";
            default:
                return "???";
            }
        }

    private:

        //! \brief Current state of the state machine.
        Scan::States m_state = IDLE;
        //! \brief Memorize the car position when the first car has been
        //! detected.
        sf::Vector2f m_position;
        //! \brief Estimation of the empty spot length.
        float m_distance = 0.0f;
        //! \brief Memorize the parking spot once detected.
        Parking m_parking;
    }; // class Scan

    // *************************************************************************
    //! \brief Main state machine for self-parking: drive along the parking,
    //! scan parked cars and detect empty spot, compute the path for parking and
    //! compute the reference speed and reference steering angle for the cruise
    //! controler.
    // *************************************************************************
    class StateMachine
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
        //! \brief Default constructor. m_parking is initialized with dummy
        //! value.
        //----------------------------------------------------------------------
        StateMachine()
            : m_parking(ParkingDimensions::get("epi.45"), sf::Vector2f(0.0f, 0.0f))
        {}

        //----------------------------------------------------------------------
        //! \brief Update the state machine.
        //! \param[in] dt: delta time in seconds.
        //! \param[inout] car: the reference of the ego car.
        //----------------------------------------------------------------------
        void update(float const dt, SelfParkingCar& car);

    private:

        //----------------------------------------------------------------------
        //! \brief For debug purpose only.
        //----------------------------------------------------------------------
        std::string to_string(SelfParkingCar::StateMachine::States s)
        {
            switch (s)
            {
            case SelfParkingCar::StateMachine::States::IDLE:
                return "IDLE";
            case SelfParkingCar::StateMachine::States::SCAN_PARKING_SPOTS:
                return "SCAN_PARKING_SPOTS";
            case SelfParkingCar::StateMachine::States::COMPUTE_ENTERING_TRAJECTORY:
                return "COMPUTE_ENTERING_TRAJECTORY";
            case SelfParkingCar::StateMachine::States::COMPUTE_LEAVING_TRAJECTORY:
                return "COMPUTE_LEAVING_TRAJECTORY";
            case SelfParkingCar::StateMachine::States::DRIVE_ALONG_TRAJECTORY:
                return "DRIVE_ALONG_TRAJECTORY";
            case SelfParkingCar::StateMachine::States::TRAJECTORY_DONE:
                return "TRAJECTORY_DONE";
            default:
                return "???";
            }
        }

        //! \brief Current state of the state machine.
        States m_state = States::IDLE;
        //! \brief Parking spot scanner state machine.
        SelfParkingCar::Scan m_scan;
        //! \brief Parking spot dimension if and only the scan found one
        Parking m_parking;
    }; // class StateMachine

public:

    //-------------------------------------------------------------------------
    //! \brief Default constructor.
    //! \param[in] dimension: dimension of the car shape.
    //! \param[in] cars: const reference to cars living in the simulation. This
    //! the simplest way to simulate the presence of existing cars.
    //-------------------------------------------------------------------------
    SelfParkingCar(CarDimension const& dimension, std::deque<std::unique_ptr<Car>> const& cars)
        : Car(dimension), m_cars(cars)
    {
        auto& sensors = m_shape.sensors();
        float const range = 10.0f;

        m_radars.resize(sensors.size());
        size_t i = sensors.size();
        while (i--)
        {
            m_radars[i].init(sensors[i], range);
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Default constructor.
    //! \param[in] model: non-NULL string car varient to get dimension of the
    //! car shape.
    //! \param[in] cars: const reference to cars living in the simulation. This
    //! the simplest way to simulate the presence of existing cars.
    //-------------------------------------------------------------------------
    SelfParkingCar(const char* model, std::deque<std::unique_ptr<Car>> const& cars)
        : SelfParkingCar(CarDimensions::get(model), cars)
    {}

    //-------------------------------------------------------------------------
    //! \brief Initialize first value for the physics equations.
    //! \param[in] position: position of the middle of the rear axle.
    //! \param[in] heading: initial yaw of the car [rad]
    //! \param[in] speed: initial longitudinal speed [m/s] (usually 0).
    //! \param[in] steering: initial front wheels orientation [rad] (usually 0).
    //-------------------------------------------------------------------------
    virtual void init(sf::Vector2f const& position, float const heading,
                      float const speed = 0.0f, float const steering = 0.0f) override
    {
        Car::init(position, heading, speed, steering);
        for (auto& it: m_radars)
        {
            it.set(position, heading);
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Check if the car detects other car.
    //! \return true if a car has been detected by one of sensors.
    //-------------------------------------------------------------------------
    bool detect() // FIXME retourner un champ de bit 1 bool par capteur
    {
        sf::Vector2f p; // TODO to be returned

        for (auto const& car: m_cars)
        {
            // TODO for the moment a single sensor
            // TODO simulate defectuous sensor
            if (m_turning_right)
            {
                assert(m_radars.size() >= CarShape::WheelName::RR);
                if (m_radars[CarShape::WheelName::RR].detects(car->obb(), p))
                    return true;
            }
            else if (m_turning_left)
            {
                assert(m_radars.size() >= CarShape::WheelName::RL);
                if (m_radars[CarShape::WheelName::RL].detects(car->obb(), p))
                    return true;
            }
        }

        return false;
    }

    //-------------------------------------------------------------------------
    //! \brief Create the trajectory to the given parking spot.
    //! \param[in] parking: the parking spot to park in.
    //! \return true if the car can park on it, else return false.
    //-------------------------------------------------------------------------
    bool park(Parking const& parking, bool const entering) // TODO proper
    {
        m_trajectory = CarTrajectory::create(parking.type);
        return m_trajectory->init(*this, parking, entering);
    }

    //-------------------------------------------------------------------------
    //! \brief Update the trajectory, cruise control, car physics ...
    //! \param[in] dt: delta time [seconds] from the previous call.
    //-------------------------------------------------------------------------
    void update(float const dt)
    {
        m_statemachine.update(dt, *this);
        m_control.update(dt);
        m_kinematic.update(m_control, dt);
    }

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

    //-------------------------------------------------------------------------
    //! \brief Update the trajectory (get current references and make it use
    //! by cruise controller).
    //-------------------------------------------------------------------------
    bool updateTrajectory(float const dt)
    {
        if (m_trajectory == nullptr)
            return true;

        return m_trajectory->update(m_control, dt);
    }

    inline std::vector<Radar> const& radars() const
    {
        return m_radars;
    }

    inline Radar const& radar(CarShape::WheelName const nth) const
    {
        assert(nth < m_radars.size());
        return m_radars[nth];
    }

private:

    //! \brief The list of parked needed for simulate radar
    std::deque<std::unique_ptr<Car>> const& m_cars; // TODO: world
    //! \brief Sensors
    std::vector<Radar> m_radars;
    //! \brief Trajectory
    std::unique_ptr<CarTrajectory> m_trajectory = nullptr;
    //! \brief Trigger for doing parking
    std::atomic<bool> m_clignotant{false};
    //! \brief
    StateMachine m_statemachine;
};

#endif
