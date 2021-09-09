// 2021 Quentin Quadrat lecrapouille@gmail.com
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

#include "CarTrajectory.hpp"
#include "TurningRadius.hpp"
#include "Renderer.hpp"
#include "Car.hpp"
#include "Utils.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

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
