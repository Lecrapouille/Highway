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

#include "SelfParking/TurningRadius.hpp"
#include "SelfParking/DiagonalTrajectory.hpp"
#include "Renderer/Renderer.hpp"
#include "Vehicle/Vehicle.hpp"
#include "Utils/Utils.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

//------------------------------------------------------------------------------
// TODO DECOUPER en 2 init
bool DiagonalTrajectory::init(Car& car, Parking const& parking, bool const entering)
{
    if (!computePathPlanning(car, parking, entering))
        return false;

    generateReferenceTrajectory(car, entering, VMAX, ADES);
    return true;
}

//------------------------------------------------------------------------------
// https://github.com/Lecrapouille/Highway/issues/33
bool DiagonalTrajectory::computePathPlanning(Car const& car, Parking const& parking, bool const entering)
{
    if (entering)
    {
        const float d = 5.0f; // distance entre voiture
        const float dv = 3.1f;//FIXME parking.dim.width * cosf(parking.dim.angle);
        std::cout << "dv: " << dv << std::endl;

        // Initial position
        Xi = car.position().x;
        Yi = car.position().y;
        std::cout << "Xi: " << Xi << ", Yi: " << Yi << std::endl;

        // (Xc, Yc) the front left corner of the car once parked
        const float K = car.dim.width / 2.0f;
        const float QQ = 0.5f;
        Xc = parking.position().x + (car.dim.length + car.dim.front_overhang + QQ) * cosf(parking.dim.angle) - K * sinf(parking.dim.angle);
        Yc = parking.position().y + (car.dim.length + car.dim.front_overhang + QQ) * sinf(parking.dim.angle) + K * cosf(parking.dim.angle);
        std::cout << "Xc: " << Xc << ", Yc: " << Yc << std::endl;

        dl = Yi - (car.dim.width / 2.0f) - Yc;
        std::cout << "dl: " << dl << std::endl;

        // 1ere manoeuver
        const float N = car.dim.length + dv * cosf(parking.dim.angle) - car.dim.back_overhang;
        Rin1 = (N * N + d * d) / (2.0f * d)     + (car.dim.width / 2.0f);
        theta1 = asinf(N / Rin1);
        beta1 = atanf(car.dim.wheelbase / (Rin1 + car.dim.length / 2.0f));

        std::cout << "Turn1: { R: " << Rin1 << ", the: " << RAD2DEG(theta1) << ", beta: " << RAD2DEG(beta1) << " }" << std::endl;

        // 2ieme maneuvre
        theta2 = parking.dim.angle - theta1; // = car.heading() a la fin de la 1ere maneuvre
        Rin2 = dl / (1.0f - cosf(theta2))       + (car.dim.width / 2.0f) ;
        beta2 = atanf(car.dim.wheelbase / (Rin2 + car.dim.length / 2.0f));

        std::cout << "Turn2: { R: " << Rin2 << ", the: " << RAD2DEG(theta2) << ", beta: " << RAD2DEG(beta2) << " }" << std::endl;

        const float dx = Rin2 * sinf(theta2);
        Xdm = Xc + dv + dx;
        Ydm = Yi;

        std::cout << "Xdm: " << Xdm << ", Ydm: " << Ydm << std::endl;

        // Dead zone ?
        TurningRadius radius(car.dim, car.dim.max_steering_angle);
        const float Rimin = radius.internal;
        const float dlmin = (1.0f - cosf(theta2)) * Rimin;
        std::cout << "dlmin: " << dlmin << std::endl;
        if (dl <= dlmin)
        {
            std::cerr << "dead zone" << std::endl;
            return false;
        }
    }
    else
    {
        std::cerr << "TBD" << std::endl;
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
void DiagonalTrajectory::generateReferenceTrajectory(Car const& car, bool const entering, float const VMAX, float const ADES)
{
    // Clear internal states
    m_speeds.clear();
    m_accelerations.clear();
    m_steerings.clear();
    m_time = 0.0f;

    // Be sure to use basolute values
    assert(VMAX > 0.0f);
    assert(ADES > 0.0f);

    if (USE_KINEMATIC)
    {
        if (entering)
        {
            float const DURATION_TO_TURN_WHEELS = 0.0f;

            // Init reference to idle the car.
            m_speeds.add(0.0f, DURATION_TO_TURN_WHEELS);
            m_steerings.add(0.0f, DURATION_TO_TURN_WHEELS);

            // Xi -> Xdm
            float t = std::abs(Xi - Xdm) / VMAX;
            m_speeds.add((Xi > Xdm) ? -VMAX : VMAX, t);
            m_steerings.add(0.0f, t);

            // Pause the car to turn the wheel
            m_speeds.add(0.0f, DURATION_TO_TURN_WHEELS);
            m_steerings.add(-beta2, DURATION_TO_TURN_WHEELS);

            // Xdm -> Xc
            t = ARC_LENGTH(theta2, Rin2) / VMAX;
            m_speeds.add(-VMAX, t);
            m_steerings.add(-beta2, t);

            // Pause the car to turn the wheel
            m_speeds.add(0.0f, DURATION_TO_TURN_WHEELS);
            m_steerings.add(-beta1, DURATION_TO_TURN_WHEELS);

            // final turn
            t = ARC_LENGTH(theta1, Rin1) / VMAX;
            m_speeds.add(-VMAX, t);
            m_steerings.add(-beta1, t);

            // Pause the car to turn the wheel
            m_speeds.add(0.0f, DURATION_TO_TURN_WHEELS);
            m_steerings.add(0.0f, DURATION_TO_TURN_WHEELS);
        }
        else
        {
            std::cerr << "TODO" << std::endl;
        }
    }
    else
    {
        std::cerr << "TODO: DiagonalTrajectory::generateReferenceTrajectory !USE_KINEMATIC"
                  << std::endl;
    }
}


//------------------------------------------------------------------------------
void DiagonalTrajectory::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Xi -> Xdm
    target.draw(Arrow(Xi, Yi, Xdm, Ydm, sf::Color::Black), states);

    // C1
    //target.draw(Arc(Xdm, Ydm - Rin2 - dl, Rin2, 90.0f, RAD2DEG(theta2), sf::Color::Red));

    // C2
    target.draw(Arc(Xdm, Ydm - Rin2, Rin2, 90.0f, RAD2DEG(theta2), sf::Color::Red));

    // Roue avant gauche Destination
    target.draw(Circle(Xc, Yc, 2.0f * OUTLINE_THICKNESS, sf::Color::Yellow), states);

    // C2
    target.draw(Circle(Xdm, Ydm, 2.0f * OUTLINE_THICKNESS, sf::Color::Yellow), states);

    /*
      target.draw(Circle(Xc1, Yc1, Rwmin, sf::Color::Red), states);
      target.draw(Arrow(Xc1, Yc1, Xc1, Yf, sf::Color::Red), states);
      target.draw(Circle(Xc2, Yc2, Rwmin, sf::Color::Blue), states);
      target.draw(Arrow(Xc2, Yc2, Xs, Ys, sf::Color::Blue), states);
      target.draw(Arrow(Xi, Yi, Xs, Ys, sf::Color::Black), states);

      target.draw(Circle(Xt, Yt, 2.0f * OUTLINE_THICKNESS, sf::Color::Yellow), states);
      target.draw(Circle(Xc1, Yf, 2.0f * OUTLINE_THICKNESS, sf::Color::Yellow), states);
    */
}
