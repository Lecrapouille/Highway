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

#include "Vehicle/VehiclePhysics.hpp"

//-----------------------------------------------------------------------------
TrailerKinematic::TrailerKinematic(TrailerShape& shape, IPhysics& front)
    : m_shape(shape)
{
    attachTo(front);
}

//-----------------------------------------------------------------------------
void TrailerKinematic::init(float const speed, float const heading)
{
    float x = 0.0f;
    float y = 0.0f;
    IPhysics* front = next;
    assert(front != nullptr);

    //while ((front != nullptr) && (front->next == nullptr))
    {
        //x += cosf(front->heading()) * m_shape.dim.wheelbase;
        //y += sinf(front->heading()) * m_shape.dim.wheelbase;
        //front = front->next;
    }

    assert(front != nullptr);
    sf::Vector2f position = front->position() - sf::Vector2f(x, y);
    m_shape.set(position, heading);
    m_speed = speed;
}

//-----------------------------------------------------------------------------
void TrailerKinematic::onUpdate(CarControl const& control, float const dt)
{
    assert(next != nullptr);
    float heading = m_shape.heading();
    float next_heading = next->heading();
    m_speed = control.outputs.body_speed;

    if (next->next == nullptr)
    {
        heading += dt * m_speed * sinf(next_heading - heading) / m_shape.dim.wheelbase;
    }
    else
    {
        std::cerr << "not yet managed" << std::endl;
        exit(1);
    }

    float x = 0.0f;
    float y = 0.0f;
    IPhysics* vehicle = next;
    //while ((vehicle != nullptr) && (vehicle->next == nullptr))
    {
        x += cosf(vehicle->heading()) * m_shape.dim.wheelbase;
        y += sinf(vehicle->heading()) * m_shape.dim.wheelbase;
        //vehicle = vehicle->next;
    }

    assert(vehicle != nullptr);
    sf::Vector2f position = vehicle->position() - sf::Vector2f(x, y);
    m_shape.set(position, heading);
}

//-----------------------------------------------------------------------------
void CarKinematic::init(sf::Vector2f const& position, float const heading, float const speed,
                        float const steering)
{
    m_shape.set(position, heading, steering);
    m_speed = speed;
}

//-----------------------------------------------------------------------------
void CarKinematic::onUpdate(CarControl const& control, float const dt)
{
    float steering = control.outputs.steering;
    float heading = m_shape.heading();
    sf::Vector2f position = m_shape.position();

    m_speed = control.outputs.body_speed;
    heading += dt * m_speed * tanf(steering) / m_shape.dim.wheelbase;
    position.x += dt * m_speed * cosf(heading);
    position.y += dt * m_speed * sinf(heading);
    m_shape.set(position, heading, steering);
}
