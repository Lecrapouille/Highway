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

#include "SelfParking/Trajectories/TurningRadius.hpp"
#include "SelfParking/Trajectories/ParallelTrajectory.hpp"
#include "Renderer/Renderer.hpp"
#include "Vehicle/Vehicle.hpp"

// A maneuver is a chnage of gear: reverse or forward
static constexpr size_t MAX_MANEUVERS = 16u;

//------------------------------------------------------------------------------
bool ParallelTrajectory::init(Car& car, Parking const& parking, bool const entering)
{
    // More the steering angle is great more the turning radius is short
    // ../../../doc/pics/TurninRadius.png
    // ../../../doc/pics/LeavingCondition.png
    TurningRadius radius(car.dim, car.dim.max_steering_angle);
    Remin = radius.external;
    Rimin = radius.internal;
    Rwmin = Rimin + car.dim.width / 2.0f;
    Lmin = car.dim.back_overhang + sqrtf(Remin * Remin - Rimin * Rimin);
    std::cout << "Steermax=" << RAD2DEG(car.dim.max_steering_angle)
              << ", Remin=" << Remin << ", Rimin=" << Rimin
              << ", Rwmin=" << Rwmin << ", Lmin=" << Lmin
              << std::endl;
    assert(Remin >= Rimin);

    // Reserve memory
    theta_E.resize(MAX_MANEUVERS);
    theta_t.resize(MAX_MANEUVERS);
    theta_p.resize(MAX_MANEUVERS);
    theta_s.resize(MAX_MANEUVERS);
    theta_g.resize(MAX_MANEUVERS);
    theta_sum.resize(MAX_MANEUVERS);
    Rrg.resize(MAX_MANEUVERS);
    C.resize(MAX_MANEUVERS);
    Em.resize(MAX_MANEUVERS);

    // Minimum length of the parallel parking length. See figure 4 "Easy
    // Path Planning and Robust Control for Automatic Parallel Parking" by
    // Sungwoo CHOI, Clément Boussard, Brigitte d’Andréa-Novel.
    //
    // We use the Pythagorean theorem of the triangle CBA (90° on B) where C
    // is the center of the turning circle, B the back-left wheel (internal
    // radius Rimin) and A the front-right wheel (external radius Remin). Since
    // the frame of the car body is placed at the center of the back axle, we
    // have to add the back overhang.
    if (parking.dim.length >= Lmin)
    {
        // TODO
    }
    else
    {
        // ../../../doc/pics/ParallelManeuversEq.png
        m_maneuvers = computePath(car, parking);
        if ((m_maneuvers > 0u) && (m_maneuvers < MAX_MANEUVERS))
        {
            generateReferences(car, parking, VMAX, ADES);
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
size_t ParallelTrajectory::computePath(Car const& car, Parking const& parking)
{
    const float PI_2 = 3.1415f / 2.0f; // pi / 2
    const float PI3_2 = 3.0f * 3.1415f / 2.0f; // 3 pi / 2

    // Count maneuvers
    size_t i = 0u;

    // See "Estimation et contrôle pour le pilotage automatique de véhicule" by
    // Sungwoo Choi. We are working with position relative to the middle of the
    // bottom side of the parking spot. The position of the middle rear axle of
    // the ego car shall touch the car parked on its back. The position is:
    Em[0].x = car.dim.back_overhang - parking.dim.length / 2.0f;
    Em[0].y = car.dim.width / 2.0f;

    // Initial position
    Xi = car.position().x;
    Yi = car.position().y;

    // Final position
    Xf = parking.position().x + parking.dim.length / 2.0f;
    Yf = parking.position().y - car.dim.width / 2.0f;

    // Give extra space to avoid the rear overhang of the ego car collides with
    // the parked car back the ego car.
    const float MARGIN = 0.0f; // [meter]

    std::cout << "Parking: " << parking.position().x << " " << parking.position().y << std::endl;
    std::cout << "Xf: " << Xf << " " << Yf << std::endl;
    std::cout << "Em0: " << Em[0].x << " " << Em[0].y << std::endl;

    while (true)
    {
        // Too many maneuvers: abort and try to find another parking spot
        if (i >= MAX_MANEUVERS - 2u)
        {
            std::cerr << "Too many maneuvers " << i << std::endl;
            return i;
        }

        // Initial iteration: the ego car is touching the rear car parked.
        // The ego car is driving forward while turning to the left until
        // touching the front car parked.
        // ../../../doc/pics/ParallelStep1.png
        else if (i == 0u)
        {
            std::cout << "#############################" << std::endl;
            std::cout << "Initial maneuver: forward + left" << std::endl;
            C[i].x = Em[i].x;
            C[i].y = Em[i].y + Rwmin;
            theta_t[i] = asinf((parking.dim.length / 2.0f - C[i].x) / Remin);
            theta_s[i] = asinf((car.dim.length + MARGIN - car.dim.back_overhang) / Remin);
            theta_E[i] = theta_t[i] - theta_s[i];
            theta_sum[i] = theta_E[i];
            Em[i + 1].x = C[i].x + Rwmin * cosf(theta_sum[i] + PI3_2);
            Em[i + 1].y = C[i].y + Rwmin * sinf(theta_sum[i] + PI3_2);

            std::cout << "C" << i+1 << ": " << C[i].x << " " << C[i].y << std::endl;
            std::cout << "Em" << i+1 << ": " << Em[i + 1].x << " " << Em[i + 1].y << std::endl;
            std::cout << "ThetaT" << i+1 << ": " << theta_t[i] << std::endl;
            std::cout << "ThetaS" << i+1 << ": " << theta_s[i] << std::endl;
            std::cout << "ThetaE" << i+1 << ": " << theta_E[i] << std::endl;

            i += 1u;
        }

        // Even iteration: The ego car is driving forward while turning to the
        // left until touching the front car parked.
        // ../../../doc/pics/ParallelStep1.png
        else if ((i & 1) == 0)
        {
            std::cout << "#############################" << std::endl;
            std::cout << "Even iteration: forward + left" << std::endl;
            C[i].x = 2.0f * Em[i].x - C[i - 1].x;
            C[i].y = 2.0f * Em[i].y - C[i - 1].y;
            theta_t[i] = asinf((parking.dim.length / 2.0f - C[i].x) / Remin);
            theta_s[i] = asinf((car.dim.length + MARGIN - car.dim.back_overhang) / Remin);
            theta_E[i] = theta_t[i] - theta_sum[i - 1] - theta_s[i];
            theta_sum[i] = theta_sum[i - 1] + theta_E[i];
            Em[i + 1].x = C[i].x + Rwmin * cosf(theta_sum[i] + PI3_2);
            Em[i + 1].y = C[i].y + Rwmin * sinf(theta_sum[i] + PI3_2);

            std::cout << "C" << i+1 << ": " << C[i].x << " " << C[i].y << std::endl;
            std::cout << "Em" << i+1 << ": " << Em[i + 1].x << " " << Em[i + 1].y << std::endl;
            std::cout << "ThetaT" << i+1 << ": " << theta_t[i] << std::endl;
            std::cout << "ThetaS" << i+1 << ": " << theta_s[i] << std::endl;
            std::cout << "ThetaE" << i+1 << ": " << theta_E[i] << std::endl;

            i += 1u;
        }

        // Odd iteration: The ego car is driving backward while turning to the
        // right until touching the rear car parked.
        // ../../../doc/pics/ParallelStep2.png
        // ../../../doc/pics/Rrg.png
        else // if ((i & 1) == 1)
        {
            std::cout << "#############################" << std::endl;
            std::cout << "Even iteration: backward + right" << std::endl;
            C[i].x = 2.0f * Em[i].x - C[i - 1].x;
            C[i].y = 2.0f * Em[i].y - C[i - 1].y;
            Rrg[i] = sqrtf(POW2(car.dim.back_overhang) + POW2(Rimin + car.dim.width));
            theta_p[i] = acosf((Rimin + car.dim.width) / Rrg[i]);
            theta_g[i] = acosf((C[i].x + parking.dim.length / 2.0f) / Rrg[i]);
            theta_E[i] = PI_2 - theta_sum[i - 1] - theta_p[i] - theta_g[i];
            theta_sum[i] = theta_sum[i - 1] + theta_E[i];
            Em[i + 1].x = C[i].x + Rwmin * cosf(theta_sum[i] + PI_2);
            Em[i + 1].y = C[i].y + Rwmin * sinf(theta_sum[i] + PI_2);

            std::cout << "C" << i+1 << ": " << C[i].x << " " << C[i].y << std::endl;
            std::cout << "Em" << i+1 << ": " << Em[i + 1].x << " " << Em[i + 1].y << std::endl;
            std::cout << "ThetaP" << i+1 << ": " << theta_p[i] << std::endl;
            std::cout << "ThetaG" << i+1 << ": " << theta_g[i] << std::endl;
            std::cout << "ThetaE" << i+1 << ": " << theta_E[i] << std::endl;

            i += 1u;

            // Can the ego car escape from the parking spot ? Meaning if the
            // Y position of the colliding point with the front parked car is
            // greater than the width of the parking spot ?
            // ../../../doc/pics/ParallelLeavingCondition.png
            float w = POW2(Remin) - POW2(C[i-1].x - parking.dim.length / 2.0f);
            std::cout << "Can leave ? " << (C[i-1].y + sqrtf(w)) << " > "
                      << parking.dim.width << "?" << std::endl;
            if ((w >= 0.0f) && (C[i-1].y + sqrtf(w) > parking.dim.width))
            {
                std::cout << "Can leave!!!!" << std::endl;
                break;
            }
        }
    }

    // Last turn for leaving the parking spot and last turn to
    // horizontalize to the road.
    // ../../../doc/pics/ParallelFinalStep.png
    std::cout << "#############################" << std::endl;
    std::cout << "Final iteration: Two last final turns" << std::endl;
    C[i].x = 2.0f * Em[i].x - C[i - 1].x;
    C[i].y = 2.0f * Em[i].y - C[i - 1].y;
    C[i + 1].y = (Yi - Yf) - Rwmin;
    Yt = (C[i].y + C[i + 1].y) / 2.0f;
    float d = POW2(Rwmin) - POW2(Yt - C[i].y);
    if (d < 0.0f)
    {
        std::cerr << "BBCar is too far away on Y-axis (greater than its turning radius)"
                  << std::endl;
        for (auto& it: C) // FIXME
        {
            it.x += Xf;
            it.y += Yf;
        }
        return 1u;
    }
    Xt = C[i].x + sqrtf(d);
    Xs = C[i + 1].x = 2.0f * Xt - C[i].x;
    Ys = Yi;
    theta_E[i + 1] = atan2f(Xt - C[i].x, C[i].y - Yt); // Final angle
    theta_E[i] = theta_E[i + 1] - theta_sum[i - 1];

    std::cout << "ThetaEn-1: " << RAD2DEG(theta_E[i]) << std::endl;
    std::cout << "ThetaEn: " << RAD2DEG(theta_E[i+1]) << std::endl;

    // From relative coordinates to real world coordinates
    for (auto& it: Em)
    {
        it.x += Xf;
        it.y += Yf;
    }
    for (auto& it: C)
    {
        it.x += Xf;
        it.y += Yf;
    }
    Xs += Xf;
    Xt += Xf;
    Yt += Yf;

    std::cout << "Em0: " << Em[0].x << ", " << Em[0].y << std::endl;
    std::cout << "Em1: " << Em[1].x << ", " << Em[1].y << std::endl;
    std::cout << "Em2: " << Em[2].x << ", " << Em[2].y << std::endl;

    std::cout << "=============================" << std::endl;
    std::cout << "Initial position: " << Xi << " " << Yi << std::endl;
    std::cout << "Turning at position: " << Xs << " " << Ys << std::endl;
    std::cout << "Center 1st turn: " << C[3].x << " " << C[3].y << std::endl;
    std::cout << "Center 2nd turn: " << C[2].x << " " << C[2].y << std::endl;
    std::cout << "Center 3th turn: " << C[1].x << " " << C[1].y << std::endl;
    std::cout << "Center 4th turn: " << C[0].x << " " << C[0].y << std::endl;
    std::cout << "Touching point: " << Xt << " " << Yt << std::endl;
    std::cout << "Angle: " << RAD2DEG(theta_E[2]) << std::endl;
    std::cout << "Final position: " << Xf << " " << Yf << std::endl;

    return i + 2;
}

//------------------------------------------------------------------------------
void ParallelTrajectory::generateReferences(Car const& car, Parking const& parking,
                                            float const VMAX, float const ADES)
{
    // Duration to turn front wheels to the maximal angle [s]
    float const DURATION_TO_TURN_WHEELS = 0.0f;
    float t;

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

    // Init car position -> position when starting the 1st turn: backward or
    // forward.
    if (Xi > Xs)
    {
        t = Xi - Xs / VMAX;
        m_speeds.add(-VMAX, t);
        m_steerings.add(0.0f, t);
    }
    else
    {
        t = Xs - Xi / VMAX;
        m_speeds.add(VMAX, t);
        m_steerings.add(0.0f, t);
    }

    // Turning circles
    size_t i = m_maneuvers;
    while (i--)
    {
        t = ARC_LENGTH(theta_E[i], Rwmin) / VMAX;
        if (i == m_maneuvers - 1u)
        {
            std::cout << "T111: " << t << std::endl;
            m_speeds.add(-VMAX, t);
            m_steerings.add(-car.dim.max_steering_angle, t);
        }
        else if (i == m_maneuvers - 2u)
        {
            std::cout << "T222: " << t << std::endl;
            m_speeds.add(-VMAX, t);
            m_steerings.add(car.dim.max_steering_angle, t);
        }
        else if ((i & 1) == 0)
        {
            std::cout << "T333: " << t << std::endl;
            // Turning to the right and move backward
            m_speeds.add(-VMAX, t);
            m_steerings.add(car.dim.max_steering_angle, t);
        }
        else
        {
            std::cout << "T444: " << t << std::endl;
            // Turning to the left and move forward
            m_speeds.add(VMAX, t);
            m_steerings.add(-car.dim.max_steering_angle, t);
        }
        m_speeds.add(0.0f, DURATION_TO_TURN_WHEELS);
        m_steerings.add(car.dim.max_steering_angle, DURATION_TO_TURN_WHEELS);
    }

    // Centering in the parking spot
    t = std::abs((parking.dim.length - car.dim.length) / 2.0f) / VMAX;
    m_speeds.add(VMAX, t);
    m_steerings.add(0.0f, t);

    // Halt the car
    m_speeds.add(0.0f, 0.0f);
    m_steerings.add(0.0f, 0.0f);
}

//------------------------------------------------------------------------------
// TODO a bouger dans Renderer
void ParallelTrajectory::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(Arc(C[0].x, C[0].y, Remin, 270.0f, RAD2DEG(theta_t[0]), sf::Color::Blue), states);
    target.draw(Arc(C[1].x, C[1].y, Rrg[1], 90.0f, RAD2DEG(theta_sum[0]), sf::Color::Red), states);
    target.draw(Arc(C[1].x, C[1].y, Rrg[1], 90.0f, RAD2DEG(theta_sum[0] + theta_E[1]), sf::Color::Red), states);
    target.draw(Arc(C[1].x, C[1].y, Rrg[1], 90.0f, RAD2DEG(theta_sum[0] + theta_E[1] + theta_p[0]), sf::Color::Red), states);
    target.draw(Arc(C[2].x, C[2].y, Rwmin, 270.0f, RAD2DEG(theta_sum[2] + theta_E[3]), sf::Color::Cyan), states);
    target.draw(Arc(C[3].x, C[3].y, Rwmin, 90.0f, RAD2DEG(theta_E[3]), sf::Color::Cyan), states);
    target.draw(Arrow(Xi, Yi, Xs, Ys, sf::Color::Black), states);

    target.draw(Circle(Em[0].x, Em[0].y, ZOOM, sf::Color::Green), states);
    target.draw(Circle(Em[1].x, Em[1].y, ZOOM, sf::Color::Green), states);
    target.draw(Circle(Em[2].x, Em[2].y, ZOOM, sf::Color::Red), states);
    target.draw(Circle(C[2].x, C[2].y, ZOOM, sf::Color::Black), states);
    target.draw(Circle(C[3].x, C[3].y, ZOOM, sf::Color::Black), states);
    target.draw(Circle(Xt, Yt, ZOOM, sf::Color::Black), states);
    target.draw(Circle(Xi, Yi, ZOOM, sf::Color::Green), states);

    /*
    if (m_maneuvers == 1u)
    {
         // Drive to initial position
        target.draw(Arrow(Xi, Yi, Xs, Ys, sf::Color::Black), states);

        // Turn 1
        target.draw(Arc(Xc2, Yc2, Rwmin, 90.0f, RAD2DEG(min_central_angle), sf::Color::Blue), states);
        target.draw(Arrow(Xc2, Yc2, Xt, Yt, sf::Color::Blue), states);

        // Turn 2
        target.draw(Arc(Xc1, Yc1, Rwmin, -90.0f, RAD2DEG(min_central_angle), sf::Color::Red), states);
        target.draw(Arrow(Xc1, Yc1, Xc1, Yf, sf::Color::Red), states);

        // Path
        target.draw(Circle(Xi, Yi, ZOOM, sf::Color::Green), states);
        target.draw(Circle(Xs, Ys, ZOOM, sf::Color::Green), states);
        target.draw(Circle(Xt, Yt, ZOOM, sf::Color::Green), states);
        target.draw(Circle(Xf, Yf, ZOOM, sf::Color::Green), states);
    }
    else if (m_maneuvers == 2u)
    {
        //return ;

        target.draw(Arrow(Xi, Yi, Xs, Ys, sf::Color::Black), states);
        target.draw(Arc(Xc1, Yc1, Remin, 270.0f, RAD2DEG(theta_t1), sf::Color::Blue), states);
        target.draw(Arc(Xc2, Yc2, Rrg, 90.0f, RAD2DEG(theta_sum[0]), sf::Color::Red), states);
        target.draw(Arc(Xc2, Yc2, Rrg, 90.0f, RAD2DEG(theta_sum[0] + theta_E2), sf::Color::Red), states);
        target.draw(Arc(Xc2, Yc2, Rrg, 90.0f, RAD2DEG(theta_sum[0] + theta_E2 + theta_p), sf::Color::Red), states);

        target.draw(Arc(Xc4, Yc4, Rwmin, 90.0f, RAD2DEG(theta_Ef), sf::Color::Cyan), states);
        target.draw(Arc(Xc3, Yc3, Rwmin, 270.0f, RAD2DEG(theta_sum2 + theta_E3), sf::Color::Cyan), states);

        target.draw(Circle(Xi, Yi, ZOOM, sf::Color::Green), states);
        target.draw(Circle(Xem0, Yem0, ZOOM, sf::Color::Green), states);
        target.draw(Circle(Xem1, Yem1, ZOOM, sf::Color::Green), states);
        target.draw(Circle(Xem2, Yem2, ZOOM, sf::Color::Red), states);

        target.draw(Circle(Xc3, Yc3, ZOOM, sf::Color::Black), states);
        target.draw(Circle(Xc4, Yc4, ZOOM, sf::Color::Black), states);
        target.draw(Circle(Xt, Yt, ZOOM, sf::Color::Black), states);
    }
    */
}
