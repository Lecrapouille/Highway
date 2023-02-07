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

#ifndef VEHICLE_HPP
#  define VEHICLE_HPP

#  include "Vehicle/Wheel.hpp"
#  include "Vehicle/VehicleBluePrint.hpp"
#  include "Vehicle/VehiclePhysics.hpp"
#  include "ECUs/TurningIndicatorECU/TurningIndicator.hpp"
#  include <functional>

#  define COLISION_COLOR sf::Color(255, 0, 0)
#  define CAR_COLOR sf::Color(25, 130, 118)
#  define EGO_CAR_COLOR sf::Color(124, 99, 197)

// TODO faire callback collides
// TODO faire get actors around the car comme std::functional

// ****************************************************************************
//! \brief
// ****************************************************************************
template<class BLUEPRINT>
class Vehicle : /*public DynamicActor,*/ public Components
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
        : blueprint(blueprint_), name(name_), color(color_),
          turningIndicator(addECU<TurningIndicatorECU>())
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
        LOGI("Sensor '%s' attached to vehicle '%s'", sensor->name.c_str(), name.c_str());
        m_sensors.push_back(sensor);
        m_shape->addSensorShape(sensor->shape);
        return *sensor;
    }

    //-------------------------------------------------------------------------
    //! \brief Shall vehicle sensor be rendered ? Call this method for i.e.,
    //! for debugging or for showing the ECU is powered on.
    //! \param[in] enable if set true then sensor are rendered else not.
    //-------------------------------------------------------------------------
    void showSensors(bool const enable = true)
    {
        for (auto& it: m_sensors)
        {
            it->renderable = enable;
        }
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
        // Update sensors. Observer pattern: feed ECUs with sensor data.
        // Note that a sensor can be used by several ECUs.
        for (auto& sensor: m_sensors)
        {
            assert(sensor != nullptr && "nullptr sensor");
            sensor->update(dt);
            sensor->notifyObservers();
        }

        // Update Electronic Control Units. They will apply control to the car.
        // TBD: ecu->update(m_control, dt); m_control is not necessary since ECU
        // knows the car and therefore m_control
        for (auto& ecu: m_ecus)
        {
            assert(ecu != nullptr && "nullptr ECU");
            ecu->update(dt);
        }

        // Vehicle control and references
        m_control->update(dt);
        // vehicle momentum
        m_physics->update(dt);
        // Wheel momentum
        update_wheels(m_physics->speed(), m_control->get_steering());
        // Update orientation of the vehicle shape
        m_shape->update(m_physics->position(), m_physics->heading());
        // Update the tracked trailer if attached
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
        // TODO: update m_trailer->physic().init(....)
    }

    //--------------------------------------------------------------------------
    //! \brief Getter: return the const reference of the container holding
    //! all the wheels.
    //--------------------------------------------------------------------------
    inline std::array<Wheel, BLUEPRINT::Where::MAX>& wheels()
    {
        return m_wheels;
    }

    //-------------------------------------------------------------------------
    //! \brief Const getter: return the const reference of the container holding
    //! all the wheels.
    //-------------------------------------------------------------------------
    inline std::array<Wheel, BLUEPRINT::Where::MAX> const& wheels() const
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

        // TODO: traillers collisions https://github.com/Lecrapouille/Highway/issues/16
        // TODO: trigger collision callback https://github.com/Lecrapouille/Highway/issues/XXXXXXXXXXXXX
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
    inline MeterPerSecondSquared const& acceleration() const
    {
        return m_physics->acceleration();
    }

    //-------------------------------------------------------------------------
    //! \brief Set the reference longitudinal speed [m/s].
    //-------------------------------------------------------------------------
    void refSpeed(MeterPerSecond const speed)
    {
        m_control->set_ref_speed(math::constrain(speed, -11.0_mps, 50.0_mps)); // -40 .. +180 km/h
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
        m_control->set_ref_steering(math::constrain(angle, -m, m));
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
    inline MeterPerSecond const& speed() const
    {
        return m_physics->speed();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the position of the middle of the rear axle
    //! inside the world coordinates.
    //--------------------------------------------------------------------------
    inline sf::Vector2<Meter> const& position() const
    {
        return m_physics->position();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading (yaw angle) [rad].
    //--------------------------------------------------------------------------
    inline Radian const& heading() const
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
        LOGI("Vehicle '%s' reacts to key %zu", name.c_str(), key);
        auto it = m_callbacks.find(key);
        if (it != m_callbacks.end())
        {
            it->second();
            return true;
        }
        return false;
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

protected:

    //! \brief Simulate Electronic Control Units.
    //! \note Shall be decalred before references to ECUs.
    std::vector<ECU*> m_ecus;

public:

    //! \brief Dimension of the vehicle
    BLUEPRINT /*const*/ blueprint; // FIXME
    //! \brief Car's name
    std::string name;
    //! \brief Current car color. Public: to allow to change it for distinguish
    //! car between them or for showing collisions ...
    sf::Color color;
    //! \brief Turning Indicator ECU
    TurningIndicatorECU& turningIndicator;

protected:

    //! \brief List of vehicle sensors
    std::vector<std::shared_ptr<Sensor>> m_sensors;
    //! \brief The shape of the vehicle, dimension, wheel positions
    std::unique_ptr<VehicleShape<BLUEPRINT>> m_shape;
    //! \brief Kinematic, Dynamic model of the vehicle
    std::unique_ptr<VehiclePhysics<BLUEPRINT>> m_physics;
    //! \brief The cruse control
    std::unique_ptr<VehicleControl> m_control;
    //! \brief Vehicle's wheels
    std::array<Wheel, BLUEPRINT::Where::MAX> m_wheels;
    //! \brief The vehicle tracked by this vehicle instance
    Vehicle* m_trailer = nullptr;
    //! \brief List of reactions to do when events occured
    std::map<size_t, Callback> m_callbacks;
    //! \brief Has car collided again an other object?
    bool m_collided = false;
};

#endif
