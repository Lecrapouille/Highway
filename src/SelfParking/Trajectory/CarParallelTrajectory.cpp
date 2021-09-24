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

#include "SelfParking/Trajectory/TurningRadius.hpp"
#include "SelfParking/Trajectory/CarTrajectory.hpp"
#include "Renderer/Renderer.hpp"
#include "Vehicle/Vehicle.hpp"

//------------------------------------------------------------------------------
bool ParallelTrajectory::init(Car& car, Parking const& parking, bool const entering)
{
    m_trials = 0u;

    // More the steering angle is great more the turning radius is short
    TurningRadius radius(car.dim, car.dim.max_steering_angle);
    Remin = radius.external;
    Rimin = radius.internal;
    Rwmin = Rimin + car.dim.width / 2.0f;

    std::cout << "Steermax=" << RAD2DEG(car.dim.max_steering_angle)
              << ", Remin=" << Remin << ", Rimin=" << Rimin
              << ", Rwmin=" << Rwmin << std::endl;

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
    std::cout << "Lmin=" << Lmin << std::endl;

    // The car wants to park ?
    if (entering)
    {
        // Has the parking spot has enough length to perform a one-trial
        // maneuver parking ?
        if (parking.dim.length >= Lmin)
        {
            // Compute the path
            if (!inPath1Trial(car, parking))
                return false;

            // Generate references
            inRef1Trial(car, VMAX, ADES);
            return true;
        }
        else
        {
            // Compute the path
            if (!inPath2Trials(car, parking))
                return false;

            // Generate references
            inRef2Trials(car, VMAX, ADES);
            return true;
        }
    }
    else
    {
        std::cerr << "N-trial leaving parallel parking not yet implemented" << std::endl;
        return false;
    }
}

//------------------------------------------------------------------------------
// Entering to parking spot in two trials
bool ParallelTrajectory::inPath2Trials(Car const& car, Parking const& parking)
{
    printf("inPath2Trials\n");
    m_trials = 2u;

    // TODO never used ?
    //Xf = parking.position().x;
    //Yf = parking.position().y;

    // NEW
    Xc1 = car.dim.back_overhang - parking.dim.length / 2.0f;
    Yc1 = car.dim.wheelbase / 2.0f + Rwmin;
    theta_E1 = asinf((parking.dim.length / 2.0f - Xc1) / Remin) -
               asinf((car.dim.wheelbase - car.dim.back_overhang) / Remin);
    theta_sum1 = theta_E1;

    printf("Xc1=%f, Yc1=%f, theta_E1=%f, theta_sum1=%f\n",
           Xc1, Yc1, RAD2DEG(theta_E1), RAD2DEG(theta_sum1));


    Xem1 = Xc1 + Rwmin * cosf(theta_sum1 + 3.0f * 3.1415f / 2.0f);
    Yem1 = Yc1 + Rwmin * sinf(theta_sum1 + 3.0f * 3.1415f / 2.0f);
    Xc2 = 2.0f * Xem1 - Xc1;
    Yc2 = 2.0f * Yem1 - Yc1;

    Rrg = sqrtf(car.dim.back_overhang * car.dim.back_overhang +
                (Rimin + car.dim.width) * (Rimin + car.dim.width));
    theta_p = acosf((Rimin + car.dim.width) / Rrg);
    theta_g = acosf((Xc2 + parking.dim.length / 2.0f) / Rrg);
    theta_E2 = 3.1415f / 2.0f - theta_sum1 - theta_p - theta_p - theta_g;

    printf("Xc2=%f, Yc2=%f, theta_E2=%f, theta_sum2=%f\n",
           Xc2, Yc2, RAD2DEG(theta_E2), RAD2DEG(theta_sum2));

    theta_sum2 = theta_sum1 + theta_E2;
    Xem2 = Xc2 + Rwmin * cosf(theta_sum2 + 3.1415f / 2.0f);
    Yem2 = Yc2 + Rwmin * sinf(theta_sum2 + 3.1415f / 2.0f);
    Xc3 = 2.0f * Xem2 - Xc2;
    Yc3 = 2.0f * Yem2 - Yc2;

    // like done in inPath1Trial
    Xi = car.position().x;
    Yi = car.position().y;

    Yc4 = Yi - Rwmin;
    Yt = (Yc3 + Yc4) / 2.0f;
    float d = Rwmin * Rwmin - (Yt - Yc3) * (Yt - Yc3);
    if (d < 0.0f)
    {
        std::cerr << "Car is too far away on Y-axis (greater than its turning radius)"
                  << std::endl;
        return false;
    }
    Xt = Xc3 + sqrtf(d);
    Xs = Xc4 = 2.0f * Xt - Xc3;
    Ys = Yi;

    theta_Ef = atan2f(Xt - Xc3, Yc3 - Yt);
    min_central_angle = theta_Ef - theta_sum2;

    std::cout << "=============================" << std::endl;
    std::cout << "Initial position: " << Xi << " " << Yi << std::endl;
    std::cout << "Turning at position: " << Xs << " " << Ys << std::endl;
    std::cout << "Center 1st turn: " << Xc4 << " " << Yc4 << std::endl;
    std::cout << "Center 2nd turn: " << Xc3 << " " << Yc3 << std::endl;
    std::cout << "Center 3th turn: " << Xc2 << " " << Yc2 << std::endl;
    std::cout << "Center 4th turn: " << Xc1 << " " << Yc1 << std::endl;
    std::cout << "Touching point: " << Xt << " " << Yt << std::endl;
    std::cout << "Angle: " << RAD2DEG(min_central_angle) << std::endl;
    std::cout << "Final position: " << Xf << " " << Yf << std::endl;
    return true;
}

//------------------------------------------------------------------------------
// Entering to parking spot in one trial
bool ParallelTrajectory::inPath1Trial(Car const& car, Parking const& parking)
{
    m_trials = 1u;

    // Initial car position: current position of the car
    Xi = car.position().x;
    Yi = car.position().y;

    // Final destination: the parking slot
    Xf = parking.position().x;
    Yf = parking.position().y;

    // C1: center of the ending turn (end position of the 2nd turning maneuver)
    Xc1 = Xf + car.dim.back_overhang;
    Yc1 = Yf + Rwmin;

    // C2: center of the starting turn (begining position of the 1st turning
    // maneuver). Note: the X-coordinate cannot yet be computed.
    Yc2 = Yi - Rwmin;

    // Tangent intersection of C1 and C2.
    Yt = (Yc1 + Yc2) / 2.0f;
    float d = Rwmin * Rwmin - (Yt - Yc1) * (Yt - Yc1);
    if (d < 0.0f)
    {
        // To fix this case: we can add a segment line to reach the two circles but who cares
        // since this happens when the car is outside the road.
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

#if 0
//------------------------------------------------------------------------------
// Leaving the parking spot in two trials
bool ParallelTrajectory::outPath2Trials(Car const& /*car*/, Parking const& /*parking*/)
{
    return false;
}

//------------------------------------------------------------------------------
// Leaving the parking spot in one trial
bool ParallelTrajectory::outPath1Trial(Car const& /*car*/, Parking const& /*parking*/)
{
    return false;
}
#endif

//------------------------------------------------------------------------------
// Generete cruise control references for entering to parking spot in two trials
void ParallelTrajectory::inRef2Trials(Car const& car, float const VMAX, float const ADES)
{
    // Duration to turn front wheels to the maximal angle [s]
    float const DURATION_TO_TURN_WHEELS = 0.0f;

    // Clear internal states
    m_speeds.clear();
    m_accelerations.clear();
    m_steerings.clear();
    m_time = 0.0f;

    // Be sure to use basolute values
    assert(VMAX > 0.0f);
    assert(ADES > 0.0f);

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
    t = ARC_LENGTH(theta_Ef, Rwmin) / VMAX;
    m_speeds.add(-VMAX, t);
    m_steerings.add(-car.dim.max_steering_angle, t);

#if 0
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
#endif

    // Init reference to idle the car
    m_speeds.add(0.0f, 0.0f);
    m_steerings.add(0.0f, 0.0f);
}

//------------------------------------------------------------------------------
// Generete cruise control references for entering to parking spot in one trial
void ParallelTrajectory::inRef1Trial(Car const& car, float const VMAX, float const ADES)
{
    // Duration to turn front wheels to the maximal angle [s]
    float const DURATION_TO_TURN_WHEELS = 0.0f;

    // Clear internal states
    m_speeds.clear();
    m_accelerations.clear();
    m_steerings.clear();
    m_time = 0.0f;

    // Be sure to use basolute values
    assert(VMAX > 0.0f);
    assert(ADES > 0.0f);

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

#if 0
//------------------------------------------------------------------------------
// Generete cruise control references for leaving parking spot in two trials
void ParallelTrajectory::outRef1Trial(Car const& car, float const VMAX, float const ADES)
{
}

//------------------------------------------------------------------------------
// Generete cruise control references for leaving parking spot in two trials
void ParallelTrajectory::outRef2Trials(Car const& car, float const VMAX, float const ADES)
{
}
#endif

//------------------------------------------------------------------------------
// TODO a bouger dans Renderer
void ParallelTrajectory::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (m_trials == 1u)
    {
        target.draw(Circle(Xc1, Yc1, Rwmin, sf::Color::Red), states);
        target.draw(Arrow(Xc1, Yc1, Xc1, Yf, sf::Color::Red), states);
        target.draw(Circle(Xc2, Yc2, Rwmin, sf::Color::Blue), states);
        target.draw(Arrow(Xc2, Yc2, Xs, Ys, sf::Color::Blue), states);
        target.draw(Arrow(Xi, Yi, Xs, Ys, sf::Color::Black), states);
        target.draw(Circle(Xt, Yt, ZOOM, sf::Color::Yellow), states);
        target.draw(Circle(Xc1, Yf, ZOOM, sf::Color::Yellow), states);
    }
    else if (m_trials == 2u)
    {
        target.draw(Circle(Xc3, Yc3, Rwmin, sf::Color::Red), states);
        target.draw(Arrow(Xc3, Yc3, Xc3, Yf, sf::Color::Red), states);
        target.draw(Circle(Xc4, Yc4, Rwmin, sf::Color::Blue), states);
        target.draw(Arrow(Xc4, Yc4, Xs, Ys, sf::Color::Blue), states);
        target.draw(Arrow(Xi, Yi, Xs, Ys, sf::Color::Black), states);
        target.draw(Circle(Xt, Yt, ZOOM, sf::Color::Yellow), states);
        target.draw(Circle(Xc1, Yf, ZOOM, sf::Color::Yellow), states);
    }
}
