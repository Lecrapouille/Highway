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

#include "Core/Simulator/Vehicle/Vehicle.hpp"
#include "Core/Simulator/Vehicle/ECUs/BodyControlModule.hpp"
#include "MyLogger/Logger.hpp"

//------------------------------------------------------------------------------
Vehicle::Vehicle(vehicle::BluePrint const& p_blueprint, const char* p_name, sf::Color const& p_color)
    : blueprint(p_blueprint), name(p_name), color(p_color), m_steering_wheel(blueprint), m_shape(*this)
{
    //m_control = std::make_unique<VehicleControl>();

    // Add ECUs
    // BCM: control indicator bulbs, beam bulbs, and brake bulbs ...
    BodyControlModule const& BCM = addECU<BodyControlModule>("BCM", indicator_stalk, m_pedal_brake, m_gearbox);

    // Turning indicators: one on each corners.
    m_shape.addLightShape("turning", "RR", blueprint.turning_indicators[vehicle::BluePrint::Where::RR],
        sf::Color::Yellow, BCM.isRightIndicatorBulbOn());
    m_shape.addLightShape("turning", "RL", blueprint.turning_indicators[vehicle::BluePrint::Where::RL],
        sf::Color::Yellow, BCM.isLeftIndicatorBulbOn());
    m_shape.addLightShape("turning", "FR", blueprint.turning_indicators[vehicle::BluePrint::Where::FR],
        sf::Color::Yellow, BCM.isRightIndicatorBulbOn());
    m_shape.addLightShape("turning", "FL", blueprint.turning_indicators[vehicle::BluePrint::Where::FL],
        sf::Color::Yellow, BCM.isLeftIndicatorBulbOn());

    // (Head) lights
    // FIXME: factorize by just changing the color instead of creating several nodes at the same position
    m_shape.addLightShape("high beam", "FL", blueprint.lights[vehicle::BluePrint::Where::FL],
        sf::Color::Yellow, BCM.isHighBeamBulbOn());
    m_shape.addLightShape("high beam", "FR", blueprint.lights[vehicle::BluePrint::Where::FR],
        sf::Color::Yellow, BCM.isHighBeamBulbOn());
    m_shape.addLightShape("low beam", "FL", blueprint.lights[vehicle::BluePrint::Where::FL],
        sf::Color(230,230,190,255), BCM.isLowBeamBulbOn());
    m_shape.addLightShape("low beam", "FR", blueprint.lights[vehicle::BluePrint::Where::FR],
        sf::Color(230,230,190,255), BCM.isLowBeamBulbOn());
    m_shape.addLightShape("rear beam", "RL", blueprint.lights[vehicle::BluePrint::Where::RL],
        sf::Color(240,90,130,255), BCM.isRearLightBulbOn());
    m_shape.addLightShape("rear beam", "RR", blueprint.lights[vehicle::BluePrint::Where::RR],
        sf::Color(240,90,130,255), BCM.isRearLightBulbOn());
    m_shape.addLightShape("brake beam", "RL", blueprint.lights[vehicle::BluePrint::Where::RL],
        sf::Color::Red, BCM.isBrakeLightBulbOn());
    m_shape.addLightShape("brake beam", "RR", blueprint.lights[vehicle::BluePrint::Where::RR],
        sf::Color::Red, BCM.isBrakeLightBulbOn());
    // TODO not well positioned
    m_shape.addLightShape("reverse beam", "RL", blueprint.lights[vehicle::BluePrint::Where::RL],
        sf::Color::White, BCM.isReverseLightBulbOn());
    m_shape.addLightShape("reverse beam", "RR", blueprint.lights[vehicle::BluePrint::Where::RR],
        sf::Color::White, BCM.isReverseLightBulbOn());
}

//------------------------------------------------------------------------------
void Vehicle::init(MeterPerSecondSquared const acceleration, MeterPerSecond const speed,
                   sf::Vector2<Meter> const& position, Radian const heading,
                   Radian const steering)
{
    assert((m_physics != nullptr) && "Please call setPhysicModel() before init()");
    m_physics->init(acceleration, speed, position, heading);
    // TODO m_control->init(0.0f, speed, position, heading);
    //this->update_wheels(speed, steering);
    // Init scene graph with orientation and position of the vehicle.
    m_shape.update();
}

//------------------------------------------------------------------------------
bool Vehicle::reactTo(size_t const key)
{
    if (auto it = m_callbacks.find(key); it != m_callbacks.end())
    {
        it->second();
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
void Vehicle::turnSteeringWheel(Radian const delta_angle)
{
    // Bicycle front wheel
    Radian wheel_angle = m_steering_wheel.turn(delta_angle);

    // Calculate Ackermann angles for each wheels
    auto inner_radius = blueprint.wheelbase / units::math::tan(wheel_angle);
    auto outer_radius = inner_radius + blueprint.width;
    m_wheels[vehicle::BluePrint::FL].steering =
        Radian(units::math::atan(blueprint.wheelbase / inner_radius));
    m_wheels[vehicle::BluePrint::FR].steering =
        Radian(units::math::atan(blueprint.wheelbase / outer_radius));
    m_wheels[vehicle::BluePrint::RL].steering = 0.0_rad;
    m_wheels[vehicle::BluePrint::RR].steering = 0.0_rad;
}

//------------------------------------------------------------------------------
void Vehicle::applyPedals(double const pedal_throttle, double const pedal_brake)
{
    // TODO assert percentage Saturation ? Strong type ?
    m_pedal_throttle = pedal_throttle;
    m_pedal_brake = pedal_brake;
}

#if 0
//------------------------------------------------------------------------------
// Method to calculate the longitudinal force based on the Magic Formula
Newton Vehicle::calculateLongitudinalForce(Tire const& tire) const
{
    return tire.peak_factor * units::math::sin(tire.shape_factor *
        units::math::atan(tire.stiffness_factor * tire.slip_ratio)) * 1.0_N;
}

//------------------------------------------------------------------------------
Newton Vehicle::calculateAeroDrag() const
{
    return 0.5 * C_d * surface_area * rho_air * speed() * speed() * 1.0_N;
}

//------------------------------------------------------------------------------
Newton Vehicle::calculateInclineForce(Radian const road_angle) const
{
    return C_r * m_mass * GRAVITY * units::math::sin(road_angle) * 1.0_N;
}

//------------------------------------------------------------------------------
Newton Vehicle::calculateBrakeForce() const
{
    return m_pedal_brake * 5000_N;
}

//------------------------------------------------------------------------------
Newton Vehicle::calculatePowertrainForce()
{
    const auto wheel_radius = blueprint.wheels[0].radius;
    const double gear_ratio = m_gearbox.getCurrentGearRatio();

    // Convert vehicle speed (m/s) to wheel angular velocity (rad/s)
    const auto wheel_angular_velocity = speed() / wheel_radius;

    // Convert wheel angular velocity to engine RPM using the gear ratio
    const auto engine_rpm = (wheel_angular_velocity * 60.0f / (2.0f * M_PI)) * gear_ratio;

    // Get engine torque from the lookup table using the engine RPM
    NewtonMeter engine_torque = m_engine.calculateTorque(m_pedal_throttle, engine_rpm);

    // Convert engine torque to wheel torque via the gearbox
    NewtonMeter wheel_torque = engine_torque * gear_ratio * transmission_efficiency;

    // Calculate driving force on the wheels (force = torque / wheel radius)
    return wheel_torque / wheel_radius;
}

//------------------------------------------------------------------------------
void Vehicle::vehicleDynamics(Second const dt)
{
    Newton total_force = calculateTireForces() - calculateAeroDrag() - 
        calculateRollingResistance() - calculateInclineForce();
    MeterPerSecondSquared acceleration = total_force / m_mass;
    velocity += acceleration * dt;
}
#endif

//------------------------------------------------------------------------------
void Vehicle::update(Second const dt)
{
    // Update sensors. Observer pattern: feed ECUs with sensor data.
    // Note that a sensor can be used by several ECUs.
    for (auto const& sensor: m_sensors)
    {
        assert(sensor != nullptr && "nullptr sensor");
        sensor->update(dt);
        //sensor->notifyObservers();
    }

    // Update Electronic Control Units. They will apply control to the car.
    // TBD: ecu->update(m_control, dt); m_control is not necessary since ECU
    // knows the car and therefore m_control
    for (auto const& [ecu_name, ecu]: m_ecus)
    {
        ecu->update(dt);
    }

    // Vehicle control and references
    // m_control->update(dt);

    // Wheel momentum
    //update_wheels(m_physics->speed(), m_control->get_steering());

    // vehicle momentum
    m_physics->update(dt);

    // Update orientation of the vehicle shape
    m_shape.update();
}

//-------------------------------------------------------------------------
//! \brief Enable/Disable sensors by iterating on them and applying a
//! condition function.
//-------------------------------------------------------------------------
void Vehicle::enableSensor(std::function<bool(Sensor const&)> const& fun) const
{
    for (auto& it: m_sensors)
    {
        it->enable = fun(*it);
    }
}