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

#include "Vehicle/VehicleShape.hpp"

//-----------------------------------------------------------------------------
TrailerShape::TrailerShape(TrailerDimension const& dimension)
    : VehicleShape(dimension)
{
    // 2 wheels
    m_wheels.resize(2);

    // Origin on the middle of the rear wheel axle
    m_obb.setSize(sf::Vector2f(dim.length, dim.width));
    m_obb.setOrigin(dim.back_overhang, m_obb.getSize().y / 2);

    // Offset along the rear axle
    const float K = dim.width / 2 - dim.wheel_width / 2;
    m_wheels[WheelName::RL].offset = sf::Vector2f(0.0f, -K);
    m_wheels[WheelName::RR].offset = sf::Vector2f(0.0f, K);

    m_wheels[WheelName::RL].steering = 0.0f;
    m_wheels[WheelName::RR].steering = 0.0f;

    //m_wheels[WheelName::RL].speed = NAN;
    //m_wheels[WheelName::RR].speed = NAN;
}

//-----------------------------------------------------------------------------
void TrailerShape::set(sf::Vector2f const& position, float const heading)
{
    m_obb.setPosition(position);
    m_obb.setRotation(RAD2DEG(heading));

    m_wheels[WheelName::RL].position = position + HEADING(m_wheels[WheelName::RL].offset, heading);
    m_wheels[WheelName::RR].position = position + HEADING(m_wheels[WheelName::RR].offset, heading);
}

//-----------------------------------------------------------------------------
CarShape::CarShape(CarDimension const& dimension)
    : VehicleShape(dimension)
{
    // Origin on the middle of the rear wheel axle
    m_obb.setSize(sf::Vector2f(dim.length, dim.width));
    m_obb.setOrigin(dim.back_overhang, m_obb.getSize().y / 2);

    // 4 wheels
    m_wheels.resize(4);

    // Offset along the rear axle
    const float K = dim.width / 2 - dim.wheel_width / 2;
    m_wheels[WheelName::FL].offset = sf::Vector2f(dim.wheelbase, K);
    m_wheels[WheelName::FR].offset = sf::Vector2f(dim.wheelbase, -K);
    m_wheels[WheelName::RL].offset = sf::Vector2f(0.0f, K);
    m_wheels[WheelName::RR].offset = sf::Vector2f(0.0f, -K);

    m_wheels[WheelName::FL].steering = 0.0f;
    m_wheels[WheelName::FR].steering = 0.0f;
    m_wheels[WheelName::RR].steering = 0.0f;
    m_wheels[WheelName::RL].steering = 0.0f;

    //wheels[FL].speed = m_wheels[FR].speed = NAN;
    //wheels[RL].speed = m_wheels[RR].speed = NAN;

#if 1 // FIXME 1 sensor for the moment
    // 4 radars: 1 one each wheel (to make simple)
    m_sensor_shapes.resize(1);
    m_sensor_shapes[0].orientation = -90.0f;
    m_sensor_shapes[0].offset = sf::Vector2f(0.0f, -K);
#else
    // 4 radars: 1 one each wheel (to make simple)
    m_sensor_shapes.resize(4);
    m_sensor_shapes[WheelName::FL].orientation = 90.0f;
    m_sensor_shapes[WheelName::FR].orientation = -90.0f;
    m_sensor_shapes[WheelName::RL].orientation = 90.0f;
    m_sensor_shapes[WheelName::RR].orientation = -90.0f;

    m_sensor_shapes[WheelName::FL].offset = sf::Vector2f(dim.wheelbase, K);
    m_sensor_shapes[WheelName::FR].offset = sf::Vector2f(dim.wheelbase, -K);
    m_sensor_shapes[WheelName::RL].offset = sf::Vector2f(0.0f, K);
    m_sensor_shapes[WheelName::RR].offset = sf::Vector2f(0.0f, -K);
#endif
}

//-----------------------------------------------------------------------------
void CarShape::set(sf::Vector2f const& position, float const heading, float const steering)
{
    m_obb.setPosition(position);
    m_obb.setRotation(RAD2DEG(heading));

    m_wheels[WheelName::FL].position = position + HEADING(m_wheels[WheelName::FL].offset, heading);
    m_wheels[WheelName::FR].position = position + HEADING(m_wheels[WheelName::FR].offset, heading);
    m_wheels[WheelName::RR].position = position + HEADING(m_wheels[WheelName::RR].offset, heading);
    m_wheels[WheelName::RL].position = position + HEADING(m_wheels[WheelName::RL].offset, heading);

    m_wheels[WheelName::FL].steering = steering;
    m_wheels[WheelName::FR].steering = steering;

    for (auto& it: m_sensor_shapes)
    {
        it.obb.setRotation(it.orientation + RAD2DEG(heading));
        it.obb.setPosition(position + HEADING(it.offset, heading));
    }
}
