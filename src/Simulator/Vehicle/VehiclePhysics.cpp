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
void TrailerKinematic::init(float const speed, float const heading)
{
    float x = 0.0f;
    float y = 0.0f;

    m_shape.set(sf::Vector2f(x, y), heading);
    m_speed = speed;

    // See "Flatness and motion Planning: the Car with n-trailers" by Pierre Rouchon ...
    // x0 - sum^{n}_{i=1}(cos(heading_i) d_i)
    // y0 - sum^{n}_{i=1}(sin(heading_i) d_i)
    // i: the nth trailer and (x0, y0) middle of the rear axle of the car
    IPhysics* front = this;
    while (front != nullptr)
    {
       if (front->next == nullptr)
       {
           x = front->position().x - x;
           y = front->position().y - y;
           std::cout << "car: '" << front->name << "': " << front->position().x << ", " << front->position().y << std::endl;
           std::cout << "=> " << x << ", " << y << std::endl;
       }
       else
       {
           TrailerKinematic* f = reinterpret_cast<TrailerKinematic*>(front);
           float d = f->m_shape.dim.wheelbase;

           std::cout << "Trailer: '" << front->name << "': " << RAD2DEG(f->heading()) << " " << d << std::endl;
           x = x + cosf(f->heading()) * d;
           y = y + sinf(f->heading()) * d;
           std::cout << "=> " << x << ", " << y << std::endl;
       }
       front = front->next;
    }
    m_shape.set(sf::Vector2f(x, y), heading);
}

//-----------------------------------------------------------------------------
void TrailerKinematic::onUpdate(CarControl const& control, float const dt)
{
    std::cout << "Trailer update " << name << std::endl;
    assert(next != nullptr);

    m_speed = control.outputs.body_speed;
    float heading = m_shape.heading();
    float x = 0.0f;
    float y = 0.0f;

    if (next->next == nullptr)
    {
        heading += dt * m_speed * sinf(next->heading() - heading) / m_shape.dim.wheelbase;
    }
    else
    {
        std::cerr << "not yet managed" << std::endl;
        exit(1);
    }

    IPhysics* front = this;
    while (front != nullptr)
    {
       if (front->next == nullptr)
       {
           x = front->position().x - x;
           y = front->position().y - y;
           std::cout << "car: '" << front->name << "': " << front->position().x << ", " << front->position().y << std::endl;
           std::cout << "=> " << x << ", " << y << std::endl;
       }
       else
       {
           TrailerKinematic* f = reinterpret_cast<TrailerKinematic*>(front);
           float heading = f->heading();
           float d = f->m_shape.dim.wheelbase;

           std::cout << "Trailer: '" << front->name << "': " << RAD2DEG(heading) << " " << d << std::endl;
           x = x + cosf(heading) * d;
           y = y + sinf(heading) * d;
           std::cout << "=> " << x << ", " << y << std::endl;
       }
       front = front->next;
    }
    m_shape.set(sf::Vector2f(x, y), heading);
}

//-----------------------------------------------------------------------------
void TricycleKinematic::init(sf::Vector2f const& position, float const heading, float const speed,
                             float const steering)
{
    m_shape.set(position, heading, steering);
    m_speed = speed;
}

//-----------------------------------------------------------------------------
void TricycleKinematic::onUpdate(CarControl const& control, float const dt)
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

//-----------------------------------------------------------------------------
void TricycleDynamic::init(sf::Vector2f const& position, float const heading, float const speed,
                           float const steering)
{
    m_shape.set(position, heading, steering);
    m_speed = speed;
}

//-----------------------------------------------------------------------------
// https://github.com/quangnhat185/Self-driving_cars_toronto_coursera/blob/master/1.%20Introduciton%20to%20Self-driving%20Cars/Longitudinal_Vehicle_Model.ipynb
void TricycleDynamic::onUpdate(CarControl const& control, float const dt)
{
    static float time = 0.0f;
    //float const dt = 0.01f;
    std::cout << "==========================" << std::endl;
    std::cout << "Throttle: " << control.inputs.throttle << std::endl;

    // Gravity [Newton]
    const float G = 9.81f;
    // Mass vehicle [kg]
    const float m = 2000.0f;

    // Gravitational force due to road inclination [Newton: N == kg.m/s^2]
    const float road_angle = 0.0f; // TODO
    const float Fg = m * G * sinf(road_angle);
    std::cout << "Fg: " << Fg << std::endl;

    // Total rolling resistance (total resistant forces along X-axis)
    const float friction_coef = 0.01f;
    const float Trr = friction_coef * fabsf(m_speed);
    std::cout << "V:" << m_speed << std::endl;
    std::cout << "Trr: " << Trr << std::endl;

    // Aerodynamic force depending on air density
    // http://www.fiches-auto.fr/articles-auto/fonctionnement-d-une-auto/s-1701-l-aerodynamisme-d-une-voiture-cx-scx-trainee-etc.php
    // http://ww2.ac-poitiers.fr/vehicules-materiels/sites/vehicules-materiels/IMG/pdf/cours_prof_1_bac_l_aerodynamique.pdf
    //const float Cx = 0.33f; // Drag coeff [No dimension]
    //const float area = 3.0f; // [m^2]
    //const float d = 1.2f; // Density of air at 20 degC [kg/m^3]
    const float K = 1.36f; //0.5f * d * Cx * area;
    const float Faero = K * m_speed * m_speed; // [N]
    std::cout << "Faero: " << Faero << std::endl;

    // Total longitudinal reistance load [N]
    const float Fload = Faero + Trr + Fg;
    std::cout << "Fload: " << Fload << std::endl;

    // Update simplifed model of the powertrain (torque for engine, clutch, gear
    // box ...). Get the wheel angular speed.
    const float Ww = m_powertrain.update(dt, 0.0f/*control.inputs.throttle*/, Fload);
    //m_shape.setWheelSpeeds(Ww);

    // Normalized wheel slip ratio
    // http://www-cdr.stanford.edu/dynamic/WheelSlip/SlmillerGerdesACC.pdf
    // https://discovery.ucl.ac.uk/id/eprint/10059709/1/RNCnew_sks_3_9_2018.pdf
    const float EPSILON = 0.0001f;
    const float s = (Ww * m_powertrain.effective_radius - m_speed) / (m_speed + EPSILON);

    // Total tire forces (traction forces along X-axis).
    const float TIRE_FORCE = 10000.0f;
    const float TIRE_FMAX = 10000.0f;
    const float Ttf = (fabsf(s) < 1.0f) ? TIRE_FORCE * s : TIRE_FMAX;

    // Longitudinal acceleration (= sum of forces)
    m_acceleration = (Ttf - Fload) / m;
    std::cout << "Acc: " << m_acceleration << std::endl;

    // Longitudinal speed
    m_speed += dt * m_acceleration;

    time += dt;
    std::cout << "Th: " << control.inputs.throttle << std::endl;
    //std::cout << time << " " << m_speed /** 3.6f*/ << ";" << std::endl;

    // Longitudinal position
    //x += -0.5f * m_acceleration * dt * dt + m_speed * dt;

    //
    //float heading = m_shape.heading();
    float steering = control.outputs.steering;
    float heading = m_shape.heading();
    heading += dt * m_speed * tanf(steering) / m_shape.dim.wheelbase;

    //
    sf::Vector2f position = m_shape.position();
    position.x += dt * m_speed * cosf(heading);
    position.y += dt * m_speed * sinf(heading);

    m_shape.set(position, heading, steering);

    // Log
    m_monitor.log(name, time,
                  control.inputs.throttle,
                  m_speed, Fg, Trr, Faero, Fload, m_acceleration);

}
