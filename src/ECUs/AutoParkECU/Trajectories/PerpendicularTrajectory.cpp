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

#include "SelfParking/Trajectories/TurningRadius.hpp"
#include "SelfParking/Trajectories/PerpendicularTrajectory.hpp"
#include "Renderer/Renderer.hpp"
#include "Vehicle/Vehicle.hpp"
#include "Utils/Utils.hpp"
#include <iostream>

//------------------------------------------------------------------------------
bool PerpTrajectory::init(Car& car, Parking const& parking, bool const entering)
{
    if (!computePathPlanning(car, parking, entering))
        return false;

    generateReferenceTrajectory(car, entering, VMAX, ADES);
    return true;
}

//------------------------------------------------------------------------------
bool PerpTrajectory::computePathPlanning(Car const& car, Parking const& parking, bool const entering)
{
    if (entering)
    {
        std::cerr << "TBD" << std::endl;
    }
    else
    {
        // More the steering angle is great more the turning radius is short
        TurningRadius radius(car.dim, car.dim.max_steering_angle);
        Rmin = radius.middle;
        Rwmin = radius.internal + car.dim.width / 2.0f;

        // Initial position: parking slot
        Xi = car.position().x;
        Yi = car.position().y;
        std::cout << "Xi: " << Xi << ", Yi: " << Yi << std::endl;

        // Distance of parking inside the parking slot
        Lz = 2.0f;//parking.dim.length - car.dim.back_overhang;

        // Final position
        Xe = Rwmin + Xi;
        Ye = Rwmin + Yi - 0.5f * sqrtf(2.0f) * Rmin;
    }

    return true;
}

//------------------------------------------------------------------------------
void PerpTrajectory::generateReferenceTrajectory(Car const& car, bool const entering, float const VMAX, float const ADES)
{
    float t;

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
            std::cerr << "TBD" << std::endl;
        }
        else
        {
            float const DURATION_TO_TURN_WHEELS = 0.0f;

            // Init reference to idle the car.
            m_speeds.add(0.0f, DURATION_TO_TURN_WHEELS);
            m_steerings.add(0.0f, DURATION_TO_TURN_WHEELS);

            // Lz
            t = Lz / VMAX;
            m_speeds.add(VMAX, t);
            m_steerings.add(0.0f, t);

            // Pause the car to turn the wheel
            m_speeds.add(0.0f, DURATION_TO_TURN_WHEELS);
            m_steerings.add(-car.dim.max_steering_angle, DURATION_TO_TURN_WHEELS);

            // Xi -> Xe
            t = ARC_LENGTH(DEG2RAD(90.0f), Rwmin) / VMAX;
            m_speeds.add(VMAX, t);
            m_steerings.add(-car.dim.max_steering_angle, t);

            // Pause the car to turn the wheel
            m_speeds.add(0.0f, DURATION_TO_TURN_WHEELS);
            m_steerings.add(0.0f, DURATION_TO_TURN_WHEELS);

            // final turn
            t = 1.0f / VMAX;
            m_speeds.add(VMAX, t);
            m_steerings.add(0.0f, t);
        }
    }
    else
    {
        std::cerr << "TODO: PerpTrajectory::generateReferenceTrajectory !USE_KINEMATIC"
                  << std::endl;
    }
}

//------------------------------------------------------------------------------
void PerpTrajectory::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    //target.draw(Arc(Xdm, Ydm - Rin2, Rin2, 90.0f, RAD2DEG(theta2), sf::Color::Red));
    //target.draw(Circle(Xdm, Ydm, 2*ZOOM, sf::Color::Yellow), states);
}
