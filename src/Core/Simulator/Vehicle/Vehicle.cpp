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
#include "MyLogger/Logger.hpp"
#include "Core/Math/Math.hpp"

//------------------------------------------------------------------------------
bool Vehicle::reactTo(size_t const key)
{
    //LOGI("Vehicle '%s' reacts to key %zu", name.c_str(), key);
    if (auto it = m_callbacks.find(key); it != m_callbacks.end())
    {
        it->second();
        return true;
    }
    return false;
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
}

//------------------------------------------------------------------------------
void Vehicle::update(Second const dt)
{
#if 0
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
#endif

    // vehicle momentum
    m_physics->update(dt);

    // Update wheel positions
    size_t i = m_wheels.size();
    while (i--)
    {
        m_wheels[i].position = position()
            + math::heading(blueprint.wheels[i].offset, heading());
    }

    // Wheel momentum
    //update_wheels(m_physics->speed(), m_control->get_steering());

    // Update orientation of the vehicle shape
    m_shape.update(position(), heading());

#if 0
    // Update the tracked trailer if attached
    if (m_trailer != nullptr)
    {
        m_trailer->update(dt);
    }
#endif
}