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

#ifndef VEHICLE_HPP
#  define VEHICLE_HPP

#  include "Simulator/Actor.hpp"
#  include "Simulator/Vehicle/Wheel.hpp"
#  include "Simulator/Vehicle/VehicleBluePrint.hpp"
#  include "Simulator/Vehicle/VehiclePhysics.hpp"
#  include "Simulator/Vehicle/ECU.hpp"
#  include "Simulator/Sensors/Radar.hpp"
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
template<class BLUEPRINT>
class Vehicle : public DynamicActor, public Components
{
public:

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    enum class TurningIndicator { Off, Left, Right, Warnings };

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
    virtual void init(float const acceleration, float const speed,
                      sf::Vector2f const& position, float const heading,
                      float const steering = 0.0f)
    {
        m_physics->init(acceleration, speed, position, heading);
        //todo m_control->init(0.0f, speed, position, heading);
        this->update_wheels(speed, steering);
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    Radar& addRadar(RadarBluePrint const& bp)
    {
        std::shared_ptr<Radar> radar = std::make_shared<Radar>(bp);
        m_sensors.push_back(radar);
        m_shape->addSensor(radar);
        return *radar;
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

    //-------------------------------------------------------------------------
    // External or internal: collides with city and sensor detecs city ???
    //-------------------------------------------------------------------------
    virtual void update(float const dt)
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
        for (auto& it: m_sensors)
            it->update(m_physics->position(), m_physics->heading());
        if (m_trailer != nullptr)
            m_trailer->update(dt);
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    virtual void update_wheels(float const speed, float const steering) = 0;

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
    inline float acceleration() const
    {
        return m_physics->acceleration();
    }

    //-------------------------------------------------------------------------
    //! \brief Set the reference longitudinal speed [m/s].
    //-------------------------------------------------------------------------
    void refSpeed(float const speed)
    {
        m_control->set_ref_speed(constrain(speed, -11.0f, 50.f)); // -40 .. +180 km/h
    }

    //-------------------------------------------------------------------------
    //! \brief Get the reference longitudinal speed [m/s].
    //-------------------------------------------------------------------------
    float refSpeed() const
    {
        return m_control->get_ref_speed();
    }

    //-------------------------------------------------------------------------
    //! \brief Set the reference steering angle [rad].
    //-------------------------------------------------------------------------
    void refSteering(float const angle)
    {
        const float m = blueprint.max_steering_angle;
        m_control->set_ref_steering(constrain(angle, -m, m));
    }

    //-------------------------------------------------------------------------
    //! \brief Get the reference steering angle [rad].
    //-------------------------------------------------------------------------
    float refSteering() const
    {
        return m_control->get_ref_steering();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the longitudinal speed [meter/second].
    //--------------------------------------------------------------------------
    inline float speed() const
    {
        return m_physics->speed();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the position of the middle of the rear axle
    //! inside the world coordinates.
    //--------------------------------------------------------------------------
    inline sf::Vector2f position() const
    {
        return m_physics->position();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading (yaw angle) [rad].
    //--------------------------------------------------------------------------
    inline float heading() const
    {
        return m_physics->heading();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the steering angle [rad].
    //--------------------------------------------------------------------------
    inline float steering() const
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
    //! \brief
    //-------------------------------------------------------------------------
    inline std::vector<std::shared_ptr<Radar>> const& sensors() const
    {
        return m_sensors;
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

    //! \brief
    std::vector<ECU*> m_ecus;
    //! \brief
    std::vector<std::shared_ptr<Radar>> m_sensors;
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
