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

#ifndef VEHICLE_HPP
#  define VEHICLE_HPP

#  include "MyLogger/Logger.hpp"
#  include "Simulator/Actor.hpp"
#  include "Simulator/Vehicle/Wheel.hpp"
#  include "Simulator/Vehicle/VehicleBluePrint.hpp"
#  include "Simulator/Vehicle/VehiclePhysics.hpp"
#  include "Simulator/Vehicle/ECU.hpp"
#  include "Simulator/Sensors/Sensor.hpp"
#  include <memory>
#  include <functional>

#  define COLISION_COLOR sf::Color(255, 0, 0)
#  define CAR_COLOR sf::Color(178, 174, 174)
#  define EGO_CAR_COLOR sf::Color(124, 99, 197)

// TODO faire callback collides
// TODO faire get actors around the car comme std::functional

// ****************************************************************************
//! \brief
// ****************************************************************************
enum class TurningIndicator { Off, Left, Right, Warnings };

// ****************************************************************************
//! \brief
// ****************************************************************************
template<class BLUEPRINT>
class Vehicle : public DynamicActor, public Components
{
public:

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    typedef std::function<void()> Callback;

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    virtual ~Vehicle() = default;

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    Vehicle(BLUEPRINT const& blueprint_, const char* name_, sf::Color const& color_)
        : blueprint(blueprint_), name(name_), color(color_)
    {
        m_shape = std::make_unique<VehicleShape<BLUEPRINT>>(blueprint);
        m_control = std::make_unique<VehicleControl>();
    }

    //-------------------------------------------------------------------------
    //! \brief Initialize first value for the physics.
    //! \param[in] position: position of the middle of the rear axle.
    //! \param[in] heading: initial yaw of the car [rad]
    //! \param[in] speed: initial longitudinal speed [m/s] (usually 0).
    //! \param[in] steering: initial front wheels orientation [rad] (usually 0).
    //-------------------------------------------------------------------------
    virtual void init(MeterPerSecondSquared const acceleration, MeterPerSecond const speed,
                      sf::Vector2<Meter> const& position, Radian const heading,
                      Radian const steering = 0.0_deg)
    {
        m_physics->init(acceleration, speed, position, heading);
        // TODO m_control->init(0.0f, speed, position, heading);
        this->update_wheels(speed, steering);
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    template<class SENSOR, class BP, typename... Args>
    SENSOR& addSensor(BP const& bp, Args&&... args)
    {
        std::shared_ptr<SENSOR> sensor = std::make_shared<SENSOR>(bp, std::forward<Args>(args)...);
        LOGI("Sensor %s attached to vehicle %s", sensor->name.c_str(), name.c_str());
        m_sensors.push_back(sensor);
        m_shape->addSensorShape(sensor->shape);
        return *sensor;
    }

    //-------------------------------------------------------------------------
    //! \brief Add a new ECU to the vehicle. Pass arguments to its constructor.
    //! \pre do not call during the update() method since iterator for m_ecu
    //! will be errneous.
    //-------------------------------------------------------------------------
    template<class T, typename... Args>
    T& addECU(Args&&... args)
    {
        // Create an or replace the old ECU
        T& ecu = addComponent<T>(/**this,*/ std::forward<Args>(args)...); // FIXME: *this is Vehicle while when calling it is Car

        // Refresh the list of ECUs owned by the vehicle
        m_ecus = getComponents<ECU>();
        return ecu;
    }

    template<class T> bool hasECU() const { return hasComponent<T>(); } // TODO iff T inherit from ECU
    template<class T> T& getECU() { return getComponent<T>(); }
    template<class T> T const& getECU() const { return getComponent<T>(); }

    //-------------------------------------------------------------------------
    // External or internal: collides with city and sensor detecs city ???
    //-------------------------------------------------------------------------
    virtual void update(Second const dt)
    {
        for (auto& it: m_ecus)
        {
            assert(it != nullptr && "nullptr ECU");
            it->update(dt);
        }

        m_control->update(dt);
        m_physics->update(dt);
        update_wheels(m_physics->speed(), m_control->get_steering());
        m_shape->update(m_physics->position(), m_physics->heading());
        //for (auto& it: m_sensors) // TBD: here loop of sensors::update() ?
        //{
            // TODO it->detects(...); for all collidable in World
        //}
        if (m_trailer != nullptr)
        {
            m_trailer->update(dt);
        }
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    virtual void update_wheels(MeterPerSecond const speed, Radian const steering) = 0;

    //-------------------------------------------------------------------------
    //! \brief Attach a trailer at the end of the link of trailers. The position
    //! of the trailer is deduced.
    //! \param[in] dimension: the dimension of the trailer.
    //! \param[in] heading: the angle between the trailer and the front vehicle.
    //-------------------------------------------------------------------------
    void track(Vehicle& vehicle)
    {
        m_trailer = &vehicle;
        // TODO: update init physic
    }

    //--------------------------------------------------------------------------
    //! \brief Getter: return the const reference of the container holding
    //! all the wheels.
    //--------------------------------------------------------------------------
    inline std::array<Wheel, BLUEPRINT::WheelName::MAX>& wheels()
    {
        return m_wheels;
    }

    //-------------------------------------------------------------------------
    //! \brief Const getter: return the const reference of the container holding
    //! all the wheels.
    //-------------------------------------------------------------------------
    inline std::array<Wheel, BLUEPRINT::WheelName::MAX> const& wheels() const
    {
        return m_wheels;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the const reference of the nth wheel.
    //--------------------------------------------------------------------------
    inline Wheel& wheel(size_t const nth)
    {
        assert(nth < m_wheels.size());
        return m_wheels[nth];
    }

    //-------------------------------------------------------------------------
    //! \brief Const getter: return the car shape.
    //-------------------------------------------------------------------------
    inline VehicleShape<BLUEPRINT> const& shape() const
    {
        return *m_shape;
    }

    //-------------------------------------------------------------------------
    //! \brief Const getter: return the oriented bounding box of the vehicle.
    //-------------------------------------------------------------------------
    inline sf::RectangleShape const& obb() const
    {
        return m_shape->obb();
    }

    //-------------------------------------------------------------------------
    //! \brief Const getter: return the oriented bounding box of the nth wheel.
    //-------------------------------------------------------------------------
    inline sf::RectangleShape obb_wheel(size_t const nth) const
    {
        // Assertion is made inside the obb_wheel() method.
        return m_shape->obb_wheel(nth, m_wheels[nth].steering);
    }

    //-------------------------------------------------------------------------
    //! \brief Check if the car or one of its trailer collides with another
    //! vehicle and one of its trailers.
    //-------------------------------------------------------------------------
    template<class T>
    bool collides(Vehicle<T>& other)
    {
        sf::Vector2f p;

        // TODO: traillers collisions
        // TODO: trigger callback
        bool res = m_shape->collides(other.obb(), p);
        m_collided |= res;
        other.m_collided |= res;
        return res;
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    inline bool collided() const
    {
        return m_collided;
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    inline void clear_collided()
    {
        m_collided = false;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return longitudinal acceleration [meter/second^2].
    //--------------------------------------------------------------------------
    inline MeterPerSecondSquared acceleration() const
    {
        return m_physics->acceleration();
    }

    //-------------------------------------------------------------------------
    //! \brief Set the reference longitudinal speed [m/s].
    //-------------------------------------------------------------------------
    void refSpeed(MeterPerSecond const speed)
    {
        m_control->set_ref_speed(speed);
            // FIXME constrain(speed, -11.0_mps, 50.0_mps)); // -40 .. +180 km/h
    }

    //-------------------------------------------------------------------------
    //! \brief Get the reference longitudinal speed [m/s].
    //-------------------------------------------------------------------------
    MeterPerSecond refSpeed() const
    {
        return m_control->get_ref_speed();
    }

    //-------------------------------------------------------------------------
    //! \brief Set the reference steering angle [rad].
    //-------------------------------------------------------------------------
    void refSteering(Radian const angle)
    {
        const Radian m = blueprint.max_steering_angle;
        m_control->set_ref_steering(constrain(angle, -m, m));
    }

    //-------------------------------------------------------------------------
    //! \brief Get the reference steering angle [rad].
    //-------------------------------------------------------------------------
    Radian refSteering() const
    {
        return m_control->get_ref_steering();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the longitudinal speed [meter/second].
    //--------------------------------------------------------------------------
    inline MeterPerSecond speed() const
    {
        return m_physics->speed();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the position of the middle of the rear axle
    //! inside the world coordinates.
    //--------------------------------------------------------------------------
    inline sf::Vector2<Meter> position() const
    {
        return m_physics->position();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading (yaw angle) [rad].
    //--------------------------------------------------------------------------
    inline Radian heading() const
    {
        return m_physics->heading();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the steering angle [rad].
    //--------------------------------------------------------------------------
    inline Radian steering() const
    {
        return m_wheels[0].steering;
    }

    //-------------------------------------------------------------------------
    //! \brief Register a callback for reacting to SFML press events.
    //-------------------------------------------------------------------------
    inline void callback(size_t const key, Callback&& cb)
    {
        m_callbacks[key] = cb;
    }

    //-------------------------------------------------------------------------
    //! \brief Call callbacks when an key was pressed (if the key was registered).
    //! \return true if the SFML I/O was known, else return false.
    //-------------------------------------------------------------------------
    bool reactTo(size_t const key)
    {
        auto it = m_callbacks.find(key);
        if (it != m_callbacks.end())
        {
            it->second();
            return true;
        }
        return false;
    }

    //-------------------------------------------------------------------------
    //! \brief Set left and right turning indicators. This is not purely for
    //! rendering cosmetic but can activate event for a state machine (ie for
    //! entering or leaving a parking slot).
    //! turningIndicators(true, false) for indicating turning left.
    //! turningIndicators(false, true) for indicating turning right.
    //! turningIndicators(true, true) for indicating warnings.
    //! turningIndicators(false, false) when not turning or not in fault.
    //-------------------------------------------------------------------------
    void turningIndicator(bool const left, bool const right)
    {
        m_turning_left = left;
        m_turning_right = right;
    }

    void turningIndicator(TurningIndicator const state)
    {
        switch (state)
        {
            case TurningIndicator::Warnings:
               m_turning_left = true;
               m_turning_right = true;
            break;
            case TurningIndicator::Left:
               m_turning_left = true;
               m_turning_right = false;
            break;
            case TurningIndicator::Right:
               m_turning_left = false;
               m_turning_right = true;
            break;
            case TurningIndicator::Off:
               m_turning_left = false;
               m_turning_right = false;
            break;
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Is the car turning left, rigth, warning or not flashing.
    //-------------------------------------------------------------------------
    TurningIndicator turningIndicator() const
    {
        if (m_turning_left)
        {
           return m_turning_right ? TurningIndicator::Warnings : TurningIndicator::Left;
        }
        return m_turning_right ? TurningIndicator::Right : TurningIndicator::Off;
    }

    //-------------------------------------------------------------------------
    //! \brief By default a vehicle to be concidered as ego shall have is name
    //! starting by "ego".
    //! FIXME https://github.com/Lecrapouille/Highway/issues/6
    //-------------------------------------------------------------------------
    virtual bool isEgo() const
    {
        return (name.size() >= 3u) && (name[0] == 'e') && (name[1] == 'g') &&
               (name[2] == 'o');
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    inline std::vector<std::shared_ptr<Sensor>> const& sensors() const
    {
        return m_sensors;
    }

    //-------------------------------------------------------------------------
    //! \brief Debug purpose only: show shape information.
    //-------------------------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, Vehicle const& vehicle)
    {
        return os << "vehicle "
                  << vehicle.name << " {" << std::endl
                  << "   position: " << vehicle.position().x << ", "
                                     << vehicle.position().y << std::endl
                  << "}";
    }

public:

    //! \brief Dimension of the vehicle
    BLUEPRINT /*const*/ blueprint; // FIXME
    //! \brief Car's name
    std::string name;
    //! \brief Current car color. Public: to allow to change it for distinguish
    //! car between them or for showing collisions ...
    sf::Color color;

protected:

    //! \brief Simulate Electronic Control Unit
    std::vector<ECU*> m_ecus;
    //! \brief List of vehicle sensors
    std::vector<std::shared_ptr<Sensor>> m_sensors;
    //! \brief The shape of the vehicle, dimension, wheel positions
    std::unique_ptr<VehicleShape<BLUEPRINT>> m_shape;
    //! \brief Kinematic, Dynamic model of the vehicle
    std::unique_ptr<VehiclePhysics<BLUEPRINT>> m_physics;
    //! \brief The cruse control
    std::unique_ptr<VehicleControl> m_control;
    //! \brief Vehicle's wheels
    std::array<Wheel, BLUEPRINT::WheelName::MAX> m_wheels;
    //! \brief The vehicle tracked by this vehicle instance
    Vehicle* m_trailer = nullptr;
    //! \brief List of reactions to do when events occured
    std::map<size_t, Callback> m_callbacks;
    //! \brief Truning indicator
    bool m_turning_left = false;
    //! \brief Truning indicator
    bool m_turning_right = false;
    //! \brief Has car collided again an other object?
    bool m_collided = false;
};

#endif
