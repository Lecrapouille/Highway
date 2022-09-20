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

#include "Vehicle/ModelPhysics/TricycleDynamic.hpp"
#include "Log/Log.hpp"

//------------------------------------------------------------------------------
float PowerTrain::update(float const dt, float const throttle,
                         /*float const brake,*/ float const load)
{
    assert((throttle >= 0.0f) && (throttle <= 1.0f));
    //assert((brake >= 0.0f) && (brake <= 1.0f));

    // Engine torque
    m_torque_engine = throttle2Torque(throttle, m_speed_engine);

    // Torque converter (Clutch)
    float torque_converter = m_gear_ratio * effective_radius * load;

    // Engine angular velocity
    float acceleration_engine =
            (m_torque_engine - torque_converter) / m_inertia;
    m_speed_engine += acceleration_engine * dt;

    // Transmission (gear box)

    // Brake
    //m_torque_brake = K * brake;

    // Wheels speed
    return m_gear_ratio * m_speed_engine;
}

//------------------------------------------------------------------------------
float PowerTrain::throttle2Torque(float const throttle, float const speed)
{
    const float C[3] = { 400.0f, 0.1f, -0.0002f };
    return throttle * (C[0] + C[1] * speed + C[2] * speed * speed);
}

//------------------------------------------------------------------------------
void TricycleDynamic::update(CarControl const& control, float const dt)
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
