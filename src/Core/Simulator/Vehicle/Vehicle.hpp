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

#pragma once

#  include "Core/Simulator/Vehicle/Wheel.hpp"
//#  include "Core/Simulator/Vehicle/PowerTrain.hpp"
#  include "Core/Simulator/Vehicle/SteeringWheel.hpp"
#  include "Core/Simulator/Vehicle/PhysicModel.hpp"
#  include "Core/Simulator/Sensors/Sensor.hpp"
#  include <SFML/Graphics/Color.hpp>

#  include <functional>
#  include <map>

// ****************************************************************************
//! \brief
// ****************************************************************************
class Vehicle
{
public:

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    using Callback = std::function<void ()>;

    //-------------------------------------------------------------------------
    //! \brief
    //! \warning After this constructor, your instance will still be partially
    //! initialized. You have to set, in this order: a physic model with
    //! setPhysicModel(), a controller with setController() and initialize speed,
    //! acceleration and wih init().
    //-------------------------------------------------------------------------
    Vehicle(vehicle::BluePrint const& p_blueprint, const char* p_name, sf::Color const& p_color);

    //-------------------------------------------------------------------------
    //! \brief Because of virtual methods.
    //-------------------------------------------------------------------------
    virtual ~Vehicle() = default;

    //-------------------------------------------------------------------------
    //! \brief Set or replace the physical model of the vehicle (kinematic,
    //! dynamic ...).
    //-------------------------------------------------------------------------
    template<class Model, typename ...Args>
    void setPhysicModel(Args&&... args)
    {
        m_physics = std::move(vehicle::PhysicModel::create<Model>(
            m_shape, std::forward<Args>(args)...));
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
                      Radian const steering = 0.0_deg);

    //-------------------------------------------------------------------------
    // External or internal: collides with city and sensors detect city ???
    //-------------------------------------------------------------------------
    virtual void update(Second const dt);

    //-------------------------------------------------------------------------
    //! \brief Add a sensor to the vehicle given its position and orientation
    //! on the vehicle.
    //-------------------------------------------------------------------------
    template<class SENSOR, typename... Args>
    SENSOR& addSensor(std::string const& p_name, sensor::BluePrint const& p_blueprint,
                      sf::Color const& p_color, Args&&... args)
    {
        //LOGI("Attaching sensor '%s' to vehicle '%s'", p_name.c_str(), name.c_str());

        auto ptr = std::make_unique<SENSOR>(p_name, p_blueprint, p_color, std::forward<Args>(args)...);
        SENSOR& sensor = *ptr;
        m_shape.addSensorShape(sensor.shape());
        m_sensors.push_back(std::move(ptr));
        return sensor;
    }

    //-------------------------------------------------------------------------
    //! \brief Register a callback for reacting to SFML press events.
    //-------------------------------------------------------------------------
    inline void addCallback(size_t const key, Callback&& cb)
    {
        m_callbacks[key] = std::move(cb);
    }

    //-------------------------------------------------------------------------
    //! \brief Call callbacks when an key was pressed (if the key was registered).
    //! \return true if the SFML I/O was known, else return false.
    //-------------------------------------------------------------------------
    bool reactTo(size_t const key);

    //-------------------------------------------------------------------------
    //! \brief Const getter: return the car shape.
    //-------------------------------------------------------------------------
    inline VehicleShape const& shape() const
    {
        return m_shape;
    }

    //-------------------------------------------------------------------------
    //! \brief Const getter: return the oriented bounding box of the vehicle.
    //-------------------------------------------------------------------------
    inline sf::RectangleShape const& obb() const
    {
        return m_shape.obb();
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    vehicle::PhysicModel const& physic() const
    {
        assert((m_physics != nullptr) && "You did not call setPhysicModel()");
        return *m_physics;
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void turnSteeringWheel(Radian const delta_angle);

    //--------------------------------------------------------------------------
    //! Method to apply forces from the pedals (acceleration and brake).
    //--------------------------------------------------------------------------
    void applyPedals(double pedalAcc, double pedalBrake);

    //--------------------------------------------------------------------------
    //! \brief Const getter: return longitudinal acceleration [meter/second^2].
    //--------------------------------------------------------------------------
    inline MeterPerSecondSquared const& acceleration() const
    {
        return m_physics->acceleration();
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
    //! \brief
    //--------------------------------------------------------------------------
    inline std::array<Wheel, vehicle::BluePrint::MAX> const& wheels() const
    {
        return m_wheels;
    }

public:

    //! \brief Dimension of the vehicle
    vehicle::BluePrint const blueprint;
    //! \brief Car's name
    std::string const name;
    //! \brief Current car color. Public: to allow to change it for distinguish
    //! car between them or for showing collisions ...
    sf::Color color;

protected:

    //! \brief Kinematic, Dynamic model of the vehicle
    std::unique_ptr<vehicle::PhysicModel> m_physics = nullptr;
    //! \brief The cruise control
    //std::unique_ptr<VehicleControl> m_control = nullptr;
    //! \brief List of vehicle sensors
    std::vector<std::unique_ptr<Sensor>> m_sensors;
    //! \brief steering wheel controlling angle of wheels.
    SteeringWheel m_steering_wheel;
    //! \brief Vehicle's wheels
    std::array<Wheel, vehicle::BluePrint::MAX> m_wheels;
    //! \brief
    //Engine m_engine;
    //Gearbox m_gearbox;
    //TorqueConverter m_torque_converter;
    //! \brief Acceleration pedal (0 to 1).
    double m_pedal_throttle = 0.0;
    //! \brief Brake pedal (0 to 1).
    double m_pedal_brake = 0.0;
    //! \brief List of reactions to do when events occurred
    std::map<size_t, Callback> m_callbacks;
    //! \brief The shape of the vehicle, dimension, wheel positions.
    VehicleShape m_shape;
};