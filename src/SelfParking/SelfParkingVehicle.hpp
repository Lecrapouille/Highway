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
#  include "SelfParking/Trajectory/CarTrajectory.hpp"
#  include "SelfParking/Trajectory/TurningRadius.hpp"

// ****************************************************************************
//! \brief Self-Parking vehicle
// ****************************************************************************
class SelfParkingCar: public Car
{
private:

    class Scan
    {
        enum States {
            NOT_FOUND, IDLE, DETECT_FIRST_CAR, DETECT_HOLE,
            DETECT_SECOND_CAR, FOUND
        };

    public:

        enum Status { WIP, DETECTED, NOT_DETECTED };

        inline void start() { m_state = IDLE; }
        Scan::Status update(float const dt, SelfParkingCar& car, Parking& parking);

    private:

        std::string to_string(SelfParkingCar::Scan::States s)
        {
            switch (s)
            {
            case SelfParkingCar::Scan::States::NOT_FOUND: return "NOT_FOUND";
            case SelfParkingCar::Scan::States::IDLE: return "IDLE";
            case SelfParkingCar::Scan::States::DETECT_FIRST_CAR: return "DETECT_FIRST_CAR";
            case SelfParkingCar::Scan::States::DETECT_HOLE: return "DETECT_HOLE";
            case SelfParkingCar::Scan::States::DETECT_SECOND_CAR: return "DETECT_SECOND_CAR";
            case SelfParkingCar::Scan::States::FOUND: return "FOUND";
            default: return "???";
            }
        }

    private:

        Scan::States m_state = IDLE;
        sf::Vector2f m_position;
        float m_distance = 0.0f;
    };

    class StateMachine
    {
        enum States {
            IDLE, SCAN_PARKING_SPOTS, COMPUTE_ENTERING_TRAJECTORY,
            COMPUTE_LEAVING_TRAJECTORY, DRIVE_ALONG_TRAJECTORY,
            TRAJECTORY_DONE
        };

    public:

        StateMachine()
            : m_parking(ParkingDimensions::get("epi.45"),
                        sf::Vector2f(0.0f, 0.0f))
        {}

        void update(float const dt, SelfParkingCar& car);

    private:

        std::string to_string(SelfParkingCar::StateMachine::States s)
        {
            switch (s)
            {
            case SelfParkingCar::StateMachine::States::IDLE: return "IDLE";
            case SelfParkingCar::StateMachine::States::SCAN_PARKING_SPOTS: return "SCAN_PARKING_SPOTS";
            case SelfParkingCar::StateMachine::States::COMPUTE_ENTERING_TRAJECTORY: return "COMPUTE_ENTERING_TRAJECTORY";
            case SelfParkingCar::StateMachine::States::COMPUTE_LEAVING_TRAJECTORY: return "COMPUTE_LEAVING_TRAJECTORY";
            case SelfParkingCar::StateMachine::States::DRIVE_ALONG_TRAJECTORY: return "DRIVE_ALONG_TRAJECTORY";
            case SelfParkingCar::StateMachine::States::TRAJECTORY_DONE: return "TRAJECTORY_DONE";
            default: return "???";
            }
        }

        States m_state = States::IDLE;
        Parking m_parking;
        SelfParkingCar::Scan m_scan;
    };

public:

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    SelfParkingCar(CarDimension const& dimension, std::deque<std::unique_ptr<Car>> const& cars)
        : Car(dimension), m_cars(cars)
    {
        auto& sensors = m_shape.sensors();
        float const range = 1.0f;

        m_radars.resize(sensors.size());
        size_t i = sensors.size();
        while (i--)
        {
            m_radars[i].init(sensors[i], range);
        }
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    SelfParkingCar(const char* model, std::deque<std::unique_ptr<Car>> const& cars)
        : SelfParkingCar(CarDimensions::get(model), cars)
    {}

    //-------------------------------------------------------------------------
    //! \brief
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
    //-------------------------------------------------------------------------
    bool detect()
    {
        sf::Vector2f p;
        //for (auto const& rad: m_radars) // TODO left or right
        {
            for (auto const& car: m_cars)
            {
                if (m_radars[0].collides(car->obb(), p))
                    return true;
            }
        }
        return false;
    }

    //-------------------------------------------------------------------------
    //! \brief Trigger the car to find an empty parking spot.
    //! \note Simulate the event when the driver has activate the flashing light
    //-------------------------------------------------------------------------
    void clignotant(bool const set)
    {
        m_clignotant = set;
    }

    bool clignotant()
    {
        return m_clignotant;
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
