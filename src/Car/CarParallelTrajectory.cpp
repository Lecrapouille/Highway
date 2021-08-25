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
bool ParallelTrajectory::init(Car& car, Parking const& parking)
{
    // More the steering angle is great more the turning radius is short
    TurningRadius radius(car.dim, car.dim.max_steering_angle);
    Remin = radius.external;
    Rimin = radius.internal;
    Rwmin = Rimin + car.dim.width / 2.0f;

    // Minimum length of the parallel parking length. See figure 4 "Easy
    // Path Planning and Robust Control for Automatic Parallel Parking" by
    // Sungwoo CHOI, Clément Boussard, Brigitte d’Andréa-Novel.
    //
    // We use the Pythagorean theorem of the triangle CBA (90° on B) where C
    // is the center of the turning circle, B the back-left wheel (internal
    // radius Rimin) and A the front-right wheel (external radius Remin). Since
    // the frame of the car body is placed at the center of the back axle, we
    // have to add the back overhang.
    assert(Remin >= Rimin);
    float Lmin = car.dim.back_overhang + sqrtf(Remin * Remin - Rimin * Rimin);

    // Has the parking spot has enough length to perform a one-trial maneuver parking ?
    if (car.estimate_parking_length() >= Lmin)
    {
        if (!computePathPlanning(car, parking))
            return false;

        generateReferenceTrajectory(car, VMAX, ADES);
        return true;
    }
    else
    {
        // Implement "Estimation et contrôle pour le pilotage automatique de
        // véhicule : Stop&Go et parking automatique"
        std::cerr << "N-trial parallel parking not yet implemented" << std::endl;
        return false;
    }
}

//------------------------------------------------------------------------------
bool ParallelTrajectory::computePathPlanning(Car const& car, Parking const& parking)
{
    // TODO Missing the case the car is not enough far away

    // Initial car position: current position of the car
    Xi = car.position().x;
    Yi = car.position().y;

    // Final destination: the parking slot
    Xf = parking.position().x;
    Yf = parking.position().x;

    // C1: center of the ending turn (end position of the 2nd turning maneuver)
    Xc1 = Xf + car.dim.back_overhang;
    Yc1 = Yf + Rwmin;

    // C2: center of the starting turn (begining position of the 1st turning
    // maneuver). Note: the X-coordinate cannot yet be computed.
    Yc2 = Yi - Rwmin;

    // Tengant intersection of C1 and C2.
    Yt = (Yc1 + Yc2) / 2.0f;
    float d = Rwmin * Rwmin - (Yt - Yc1) * (Yt - Yc1);
    if (d < 0.0f)
    {
        std::cerr << "Car is too far away on Y-axis (greater than its turning radius)"
                  << std::endl;
        return false;
    }
    Xt = Xc1 + sqrtf(d);

    // Position of the car for starting the 1st turn.
    Xs = 2.0f * Xt - Xc1;
    Ys = Yi;

    // X position of C1.
    Xc2 = Xs;

    // Minimal central angle for making the turn = atanf((Xt - Xc1) / (Yc1 - Yt))
    min_central_angle = atan2f(Xt - Xc1, Yc1 - Yt);

    std::cout << "=============================" << std::endl;
    std::cout << "Initial position: " << Xi << " " << Yi << std::endl;
    std::cout << "Turning at position: " << Xs << " " << Ys << std::endl;
    std::cout << "Center 1st turn: " << Xc2 << " " << Yc2 << std::endl;
    std::cout << "Center 2nd turn: " << Xc1 << " " << Yc1 << std::endl;
    std::cout << "Touching point: " << Xt << " " << Yt << std::endl;
    std::cout << "Angle: " << RAD2DEG(min_central_angle) << std::endl;
    std::cout << "Final position: " << Xf << " " << Yf << std::endl;

    return true;
}

//------------------------------------------------------------------------------
void ParallelTrajectory::generateReferenceTrajectory(Car const& car, float const VMAX, float const ADES)
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
        // Duration to turn front wheels to the maximal angle [s]
        float const DURATION_TO_TURN_WHEELS = 0.0f;

        // Init reference to idle the car.
        m_speeds.add(0.0f, DURATION_TO_TURN_WHEELS);
        m_steerings.add(0.0f, DURATION_TO_TURN_WHEELS);

        // Init car position -> position when starting the 1st turn
        float t = std::abs(Xi - Xs) / VMAX;
        m_speeds.add(-VMAX, t);
        m_steerings.add(0.0f, t);

        // Pause the car to turn the wheel
        m_speeds.add(0.0f, DURATION_TO_TURN_WHEELS);
        m_steerings.add(-car.dim.max_steering_angle, DURATION_TO_TURN_WHEELS);

        // 1st turn: start position -> P
        t = ARC_LENGTH(min_central_angle, Rwmin) / VMAX;
        m_speeds.add(-VMAX, t);
        m_steerings.add(-car.dim.max_steering_angle, t);

        // Pause the car to turn the wheel
        m_speeds.add(0.0f, DURATION_TO_TURN_WHEELS);
        m_steerings.add(car.dim.max_steering_angle, DURATION_TO_TURN_WHEELS);

        // 2nd turn: P -> final
        m_speeds.add(-VMAX, t);
        m_steerings.add(car.dim.max_steering_angle, t);

        // Pause the car to turn the wheel
        m_speeds.add(0.0f, DURATION_TO_TURN_WHEELS);
        m_steerings.add(0.0f, DURATION_TO_TURN_WHEELS);

        // Centering in the parking spot
        t = std::abs((5.0f/*FIXME PARKING_BATAILLE_LENGTH*/ - car.dim.length) / 2.0f) / VMAX;
        m_speeds.add(VMAX, t);
        m_steerings.add(0.0f, t);

        // Init reference to idle the car
        m_speeds.add(0.0f, 0.0f);
        m_steerings.add(0.0f, 0.0f);
    }
    else
    {
        // Kinematics equations
        // http://www.ilectureonline.com/lectures/subject/PHYSICS/1/9/259
        //   acceleration: a, m_time: t
        //   velocity: v = v0 + a * t
        //   position: x = x0 + v0 * t + 0.5 * a * t * t
        // Initial values:
        //   x0 = 0 since we deal distances.
        //   v0 = 0 since to do its maneuver the car has to halt.
        // Therefore:
        //   v = a t
        //   x = a t^2 / 2
        float const t1 = VMAX / ADES;
        float const d1 = 0.5f * ADES * t1 * t1;

        // Arc length for doing a turn and m_time needed at constant velocity Vmax for
        // doing the turn.
        float const d2 = ARC_LENGTH(min_central_angle, Rwmin);
        float const t2 = (d2 - 2.0f * d1) / VMAX;

        // Maneuver Xi => Xs
        float const di = std::abs(Xi - Xs);
        float const ti = (di - 2.0f * d1) / VMAX;

        // Maneuver for centering the car in the spot
        float const d3 = std::abs((5.0f/*PARKING_BATAILLE_LENGTH*/ - car.dim.length) / 2.0f);
        float const t3 = d3 / VMAX;

        // Duration to turn front wheels to the maximal angle [s]
        float const DURATION_TO_TURN_WHEELS = 1.5f;
        float const AW = car.dim.max_steering_angle / DURATION_TO_TURN_WHEELS; // [rad/s]

        // Init
        m_accelerations.add(0.0f, 0.0f);
        m_steerings.add(0.0f, 0.0f);

        // Init car position -> position when starting the 1st turn
        m_accelerations.add((Xi >= Xs ? -ADES : ADES), t1);
        m_steerings.add(0.0f, t1);

        m_accelerations.add(0.0f, ti);
        m_steerings.add(0.0f, ti);

        m_accelerations.add((Xi >= Xs ? ADES : -ADES), t1);
        m_steerings.add(0.0f, t1);

        // Turn wheel
        m_accelerations.add(0.0f, DURATION_TO_TURN_WHEELS);
        m_steerings.add(-AW, DURATION_TO_TURN_WHEELS);

        // Turn 1
        m_accelerations.add(-ADES, t1);
        m_steerings.add(0.0f, t1);
        m_accelerations.add(0.0f, t2);
        m_steerings.add(0.0f, t2);
        m_accelerations.add(ADES, t1);
        m_steerings.add(0.0f, t1);

        // Turn wheel
        m_accelerations.add(0.0f, DURATION_TO_TURN_WHEELS);
        m_accelerations.add(0.0f, DURATION_TO_TURN_WHEELS);
        m_steerings.add(AW, DURATION_TO_TURN_WHEELS);
        m_steerings.add(AW, DURATION_TO_TURN_WHEELS);

        // Turn 2
        m_accelerations.add(-ADES, t1);
        m_steerings.add(0.0f, t1);
        m_accelerations.add(0.0f, t2);
        m_steerings.add(0.0f, t2);
        m_accelerations.add(ADES, t1);
        m_steerings.add(0.0f, t1);

        // Turn wheel
        m_accelerations.add(0.0f, DURATION_TO_TURN_WHEELS);
        m_steerings.add(-AW, DURATION_TO_TURN_WHEELS);

        // Centering in the parking spot
        m_accelerations.add(ADES, t3);
        m_steerings.add(0.0f, t3);
        m_accelerations.add(-ADES, t3);
        m_steerings.add(0.0f, t3);

        // Final
        m_accelerations.add(0.0f, DURATION_TO_TURN_WHEELS);
        m_steerings.add(0.0f, DURATION_TO_TURN_WHEELS);
    }
}

//------------------------------------------------------------------------------
// TODO a bouger dans Renderer
void ParallelTrajectory::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(Circle(Xc1, Yc1, Rwmin, sf::Color::Red), states);
    target.draw(Arrow(Xc1, Yc1, Xc1, Yf, sf::Color::Red), states);
    target.draw(Circle(Xc2, Yc2, Rwmin, sf::Color::Blue), states);
    target.draw(Arrow(Xc2, Yc2, Xs, Ys, sf::Color::Blue), states);
    target.draw(Arrow(Xi, Yi, Xs, Ys, sf::Color::Black), states);

    target.draw(Circle(Xt, Yt, 2*ZOOM, sf::Color::Yellow), states);
    target.draw(Circle(Xc1, Yf, 2*ZOOM, sf::Color::Yellow), states);
}
