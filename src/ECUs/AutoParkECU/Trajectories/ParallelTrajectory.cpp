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

#include "Simulator/Vehicle/TurningRadius.hpp"
#include "Vehicle/Car.hpp"
#include "Vehicle/ECU.hpp"
#include "ECUs/AutoParkECU/Trajectories/ParallelTrajectory.hpp"
#include "Renderer/Renderer.hpp"
#include <iostream>
#include <cassert>

// Reserve enough memory to store all maneuver states while not information are
// needed to be saved we keep them for the debug.
static constexpr size_t MAX_MANEUVERS = 64u;
static constexpr size_t TWO_LAST_TURNS = 2u;
static_assert(MAX_MANEUVERS > TWO_LAST_TURNS, "Bad value for MAX_MANEUVERS");

//------------------------------------------------------------------------------
bool ParallelTrajectory::init(Car& car, Parking const& parking, bool const entering)
{
    // We suppose that the parking spot can hold the ego car. This case is
    // supposed to be checked by the caller function.
    assert(car.blueprint.length < parking.blueprint.length);

    // More the steering angle is great more the turning radius is short
    // ../../../doc/pics/TurninRadius.png
    // ../../../doc/pics/LeavingCondition.png
    TurningRadius radius(car.blueprint, car.blueprint.max_steering_angle);
    Remin = radius.external;
    Rimin = radius.internal;
    Rwmin = Rimin + car.blueprint.width / 2.0f;
    Lmin = car.blueprint.back_overhang + units::math::sqrt(Remin * Remin - Rimin * Rimin);
    std::cout << "Steermax=" << Degree(car.blueprint.max_steering_angle)
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

    // References
    const MeterPerSecond VMAX = 1.0_mps; // Max speed [m/s]
    const MeterPerSecondSquared ADES = 1.0_mps_sq; // Desired acceleration [m/s/s]

    // Minimum length of the parallel parking length. See figure 4 "Easy
    // Path Planning and Robust Control for Automatic Parallel Parking" by
    // Sungwoo CHOI, Clément Boussard, Brigitte d’Andréa-Novel.
    //
    // We use the Pythagorean theorem of the triangle CBA (90° on B) where C
    // is the center of the turning circle, B the back-left wheel (internal
    // radius Rimin) and A the front-right wheel (external radius Remin). Since
    // the frame of the car body is placed at the center of the back axle, we
    // have to add the back overhang.
    if (parking.blueprint.length >= Lmin)
    {
        // ../../../doc/pics/ParallelFinalStep.png
        m_maneuvers = computePath1Trial(car, parking);
        if (m_maneuvers == TWO_LAST_TURNS)
        {
            generateReferences(car, parking, VMAX, ADES);
            return true;
        }
    }
    else
    {
        // ../../../doc/pics/ParallelManeuversEq.png
        m_maneuvers = computePathNTrials(car, parking);
        if ((m_maneuvers >= TWO_LAST_TURNS) && (m_maneuvers < MAX_MANEUVERS))
        {
            generateReferences(car, parking, VMAX, ADES);
            return true;
        }
    }

    m_maneuvers = 0u;
    return false;
}

//------------------------------------------------------------------------------
size_t ParallelTrajectory::computePath1Trial(Car const& car, Parking const& parking)
{
    m_ecu.reactTo(ECU::Event::Message, "1-trial maneuver");

    // Initial car position: current position of the car
    Xi = car.position().x;
    Yi = car.position().y;

    // Final destination: the parking slot
    Xf = parking.origin().x;
    Yf = parking.origin().y;

    // C1: center of the ending turn (end position of the 2nd turning maneuver)
    C[0].x = Xf + car.blueprint.back_overhang;
    C[0].y = Yf + Rwmin;

    // C2: center of the starting turn (begining position of the 1st turning
    // maneuver). Note: the X-coordinate cannot yet be computed.
    C[1].y = Yi - Rwmin;

    // Tangent intersection of C1 and C2.
    Yt = (C[0].y + C[1].y) / 2.0f;
    auto const d = Rwmin * Rwmin - (Yt - C[0].y) * (Yt - C[0].y);
    if (d.value() < 0.0)
    {
        // To fix this case: we can add a segment line to reach the two circles but who cares
        // since this happens when the car is outside the road.
        m_ecu.reactTo(ECU::Event::Message, "Car is too far away on Y-axis (greater than its turning radius)");
        return 0u;
    }
    Xt = C[0].x + units::math::sqrt(d);

    // Position of the car for starting the 1st turn.
    Xs = 2.0f * Xt - C[0].x;
    Ys = Yi;

    // X position of C1.
    C[1].x = Xs;

    // Minimal central angle for making the turn = atanf((Xt - C[0].x) / (C[0].y - Yt))
    theta_E[1] = theta_E[0] = units::math::atan2(Xt - C[0].x, C[0].y - Yt);

    std::cout << "#############################" << std::endl;
    std::cout << "Initial position: " << Xi << " " << Yi << std::endl;
    std::cout << "Turning at position: " << Xs << " " << Ys << std::endl;
    std::cout << "Center 1st turn: " << C[1].x << " " << C[1].y << std::endl;
    std::cout << "Center 2nd turn: " << C[0].x << " " << C[0].y << std::endl;
    std::cout << "Touching point: " << Xt << " " << Yt << std::endl;
    std::cout << "Angle: " << Degree(theta_E[0]) << std::endl;
    std::cout << "Final position: " << Xf << " " << Yf << std::endl;

    return 2u;
}

//------------------------------------------------------------------------------
// FIXME https://github.com/Lecrapouille/Highway/issues/27
size_t ParallelTrajectory::computePathNTrials(Car const& car, Parking const& parking)
{
    m_ecu.reactTo(ECU::Event::Message, "N-trial maneuvers");

    const Radian PI_2 = 180.0_deg / 2.0; // pi / 2
    const Radian PI3_2 = 3.0 * 180.0_deg / 2.0; // 3 pi / 2

    // Count maneuvers
    size_t i = 0u;

    // See "Estimation et contrôle pour le pilotage automatique de véhicule" by
    // Sungwoo Choi. We are working with position relative to the middle of the
    // bottom side of the parking spot. The position of the middle rear axle of
    // the ego car shall touch the car parked on its back. The position is:
    Em[0].x = car.blueprint.back_overhang - parking.blueprint.length / 2.0f;
    Em[0].y = car.blueprint.width / 2.0f;

    // Initial position
    Xi = car.position().x;
    Yi = car.position().y;

    // Final position
    Xf = parking.origin().x + parking.blueprint.length / 2.0f;
    Yf = parking.origin().y - car.blueprint.width / 2.0f;

    // Give extra space to avoid the rear overhang of the ego car collides with
    // the parked car back the ego car.
    //const float MARGIN = 0.0f; // [meter]

    std::cout << "Parking: " << parking.origin().x << " " << parking.origin().y << std::endl;
    std::cout << "Xf: " << Xf << " " << Yf << std::endl;
    std::cout << "Em0: " << Em[0].x << " " << Em[0].y << std::endl;

    while (true)
    {
        // Too many maneuvers: abort and try to find another parking spot
        if (i + TWO_LAST_TURNS >= MAX_MANEUVERS)
        {
            std::stringstream ss;
            ss << "Too many maneuvers needed to park (" << i << ". max: " << MAX_MANEUVERS << ")";
            m_ecu.reactTo(ECU::Event::Message, ss.str());
            return 0u;
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
            theta_t[i] = units::math::asin((parking.blueprint.length / 2.0f - C[i].x) / Remin);
            theta_s[i] = units::math::asin((car.blueprint.length - car.blueprint.back_overhang) / Remin);
            theta_E[i] = theta_t[i] - theta_s[i];
            theta_sum[i] = theta_E[i];
            Em[i + 1].x = C[i].x + Rwmin * units::math::cos(theta_sum[i] + PI3_2);
            Em[i + 1].y = C[i].y + Rwmin * units::math::sin(theta_sum[i] + PI3_2);

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
            theta_t[i] = units::math::asin((parking.blueprint.length / 2.0f - C[i].x) / Remin);
            theta_s[i] = units::math::asin((car.blueprint.length - car.blueprint.back_overhang) / Remin);
            theta_E[i] = theta_t[i] - theta_sum[i - 1] - theta_s[i];
            theta_sum[i] = theta_sum[i - 1] + theta_E[i];
            Em[i + 1].x = C[i].x + Rwmin * units::math::cos(theta_sum[i] + PI3_2);
            Em[i + 1].y = C[i].y + Rwmin * units::math::sin(theta_sum[i] + PI3_2);

            std::cout << "C" << i+1 << ": " << C[i].x << " " << C[i].y << std::endl;
            std::cout << "Em" << i+1 << ": " << Em[i + 1].x << " " << Em[i + 1].y << std::endl;
            std::cout << "ThetaT" << i+1 << ": " << theta_t[i] << std::endl;
            std::cout << "ThetaS" << i+1 << ": " << theta_s[i] << std::endl;
            std::cout << "ThetaE" << i+1 << ": " << theta_E[i] << std::endl;

            // Can the ego car escape from the parking spot ? Meaning if the
            // Y position of the colliding point with the front parked car is
            // greater than the width of the parking spot ?
            // ../../../doc/pics/ParallelLeavingCondition.png
            Meter x = C[i].x;
            Meter y = C[i].y;

            const auto w = units::math::pow<2>(Remin) - units::math::pow<2>((parking.blueprint.length / 2.0f) - x);
            std::cout << "Can leave ? " << (y - units::math::sqrt(w)) << " > " << parking.blueprint.width << "?" << std::endl;
            std::cout << "Putain C.x=" << x << ", C.y=" << y << ", Remin=" << Remin << std::endl;
            if ((w.value() >= 0.0) && (y - units::math::sqrt(w) > parking.blueprint.width))
            {
                m_ecu.reactTo(ECU::Event::Message, "Can leave!!!!");
                break;
            }

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
            Rrg[i] = units::math::sqrt(units::math::pow<2>(car.blueprint.back_overhang) + units::math::pow<2>(Rimin + car.blueprint.width));
            std::cout << (Rimin + car.blueprint.width) / Rrg[i] << ", " << (C[i].x + parking.blueprint.length / 2.0f) / Rrg[i] << std::endl;
            theta_p[i] = units::math::acos((Rimin + car.blueprint.width) / Rrg[i]);
            theta_g[i] = units::math::acos((C[i].x + parking.blueprint.length / 2.0f) / Rrg[i]);
            theta_E[i] = PI_2 - theta_sum[i - 1] - theta_p[i] - theta_g[i];
            theta_sum[i] = theta_sum[i - 1] + theta_E[i];
            Em[i + 1].x = C[i].x + Rwmin * units::math::cos(theta_sum[i] + PI_2);
            Em[i + 1].y = C[i].y + Rwmin * units::math::sin(theta_sum[i] + PI_2);

            std::cout << "Rrg" << i+1 << ": " << Rrg[i] << std::endl;
            std::cout << "C" << i+1 << ": " << C[i].x << " " << C[i].y << std::endl;
            std::cout << "Em" << i+1 << ": " << Em[i + 1].x << " " << Em[i + 1].y << std::endl;
            std::cout << "ThetaP" << i+1 << ": " << theta_p[i] << std::endl;
            std::cout << "ThetaG" << i+1 << ": " << theta_g[i] << std::endl;
            std::cout << "ThetaE" << i+1 << ": " << theta_E[i] << std::endl;
            i += 1u;
        }
    }

    // Last turn for leaving the parking spot and last turn to
    // horizontalize to the road. This code is the same than computePath1trial()
    // ../../../doc/pics/ParallelFinalStep.png
    std::cout << "#############################" << std::endl;
    std::cout << "Final iteration: Two last final turns" << std::endl;
    C[i].x = 2.0f * Em[i].x - C[i - 1].x;
    C[i].y = 2.0f * Em[i].y - C[i - 1].y;
    C[i + 1].y = (Yi - Yf) - Rwmin;
    Yt = (C[i].y + C[i + 1].y) / 2.0f;
    const auto d = units::math::pow<2>(Rwmin) - units::math::pow<2>(Yt - C[i].y);
    if (d.value() < 0.0)
    {
        m_ecu.reactTo(ECU::Event::Message, "Car is too far away on Y-axis (greater than its turning radius)");
        return 0u;
    }
    Xt = C[i].x + units::math::sqrt(d);
    Xs = C[i + 1].x = 2.0f * Xt - C[i].x;
    Ys = Yi;
    theta_E[i + 1] = units::math::atan2(Xt - C[i].x, C[i].y - Yt); // Final angle
    theta_E[i] = theta_E[i + 1] - theta_sum[i - 1];

    std::cout << "ThetaEn-1: " << Degree(theta_E[i]) << std::endl;
    std::cout << "ThetaEn: " << Degree(theta_E[i+1]) << std::endl;

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
    Xf = parking.origin().x;
    Yf = parking.origin().y;

    std::cout << "Em0: " << Em[0].x << ", " << Em[0].y << std::endl;
    std::cout << "Em1: " << Em[1].x << ", " << Em[1].y << std::endl;
    std::cout << "Em2: " << Em[2].x << ", " << Em[2].y << std::endl;

    std::cout << "#############################" << std::endl;
    std::cout << "Initial position: " << Xi << " " << Yi << std::endl;
    std::cout << "Turning at position: " << Xs << " " << Ys << std::endl;
    std::cout << "Center 1st turn: " << C[3].x << " " << C[3].y << std::endl;
    std::cout << "Center 2nd turn: " << C[2].x << " " << C[2].y << std::endl;
    std::cout << "Center 3th turn: " << C[1].x << " " << C[1].y << std::endl;
    std::cout << "Center 4th turn: " << C[0].x << " " << C[0].y << std::endl;
    std::cout << "Touching point: " << Xt << " " << Yt << std::endl;
    std::cout << "Angle: " << Degree(theta_E[2]) << std::endl;
    std::cout << "Final position: " << Xf << " " << Yf << std::endl;

    return i + TWO_LAST_TURNS;
}

//------------------------------------------------------------------------------
void ParallelTrajectory::generateReferences(Car const& car, Parking const& parking,
                                            MeterPerSecond const VMAX, MeterPerSecondSquared const ADES)
{
    // Duration to turn front wheels to the maximal angle [s]
    Second const DURATION_TO_TURN_WHEELS = 0.5_s;
    Second t;

    // Clear internal states
    m_speeds.clear();
    m_accelerations.clear();
    m_steerings.clear();
    m_time = 0.0_s;

    // Be sure to use basolute values
    assert(VMAX > 0.0_mps);
    assert(ADES > 0.0_mps_sq);

    // Initial reference to be sure the car starts without speed.
    m_speeds.add(0.0_mps, DURATION_TO_TURN_WHEELS);
    m_steerings.add(0.0_deg, DURATION_TO_TURN_WHEELS);

    // Initial car position: drive to the position for doing the first turn if
    // Xi > Xs the drive in reverse else drive forward. Note: We suppose Yi = Ys.
    if (Xi > Xs)
    {
        // Time to drive to destination with VMAX as longitudinal speed and
        // without turning wheels.
        t = (Xi - Xs) / VMAX;
        m_speeds.add(-VMAX, t);
        m_steerings.add(0.0_deg, t);
    }
    else
    {
        t = (Xs - Xi) / VMAX;
        m_speeds.add(VMAX, t);
        m_steerings.add(0.0_deg, t);
    }

    // Stop the car to make turn wheels
    m_speeds.add(0.0_mps, DURATION_TO_TURN_WHEELS);
    m_steerings.add(car.blueprint.max_steering_angle, DURATION_TO_TURN_WHEELS);

    // Turning circles: Consome path starting from the latest
    size_t i = m_maneuvers;
    while (i--)
    {
        // Time to drive along the arc of circle with VMAX as longitudinal speed
        t = math::arc_length(theta_E[i], Rwmin) / VMAX;

        // Lastest turn to make the ego car parallel to the road
        if (i == m_maneuvers - 1u)
        {
            std::cout << "T111: " << t << std::endl;
            m_speeds.add(-VMAX, t);
            m_steerings.add(-car.blueprint.max_steering_angle, t);
        }

        // Lastest turn to leave the parking spot
        else if (i == m_maneuvers - 2u)
        {
            std::cout << "T222: " << t << std::endl;
            m_speeds.add(-VMAX, t);
            m_steerings.add(car.blueprint.max_steering_angle, t);
        }

        // Driving forward while turning to the left
        else if ((i & 1) == 0)
        {
            std::cout << "T333: " << t << std::endl;
            m_speeds.add(-VMAX, t);
            m_steerings.add(car.blueprint.max_steering_angle, t);
        }

        // Driving backward while turning to the right
        else
        {
            std::cout << "T444: " << t << std::endl;
            m_speeds.add(VMAX, t);
            m_steerings.add(-car.blueprint.max_steering_angle, t);
        }

        // Stop the car to make turn wheels
        m_speeds.add(0.0_mps, DURATION_TO_TURN_WHEELS);
        m_steerings.add(car.blueprint.max_steering_angle, DURATION_TO_TURN_WHEELS);
    }

    // Centering the car inside its parking spot
    t = units::math::abs((parking.blueprint.length - car.blueprint.length) / 2.0f) / VMAX;
    m_speeds.add(VMAX, t);
    m_steerings.add(0.0_deg, t);

    // Halt the car
    m_speeds.add(0.0_mps, 0.0_s);
    m_steerings.add(0.0_deg, 0.0_s);
}

//------------------------------------------------------------------------------
void ParallelTrajectory::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Number of points composing the circle for the rendering
    constexpr size_t points = 32u;
    // Arc() constructor takes the starting angle and the ending angle, while
    // in this function theta_xx[] are relative angles. Add this macro to hide
    // the addition.
    #define ARC(x,y,r,s,t,c) Arc(x, y, r, s, s + t, c, points)

    if (m_maneuvers < TWO_LAST_TURNS)
        return ;

    if (m_maneuvers == TWO_LAST_TURNS)
    {
        // Drive to the position for doing the first turn.
        target.draw(Arrow(Xi, Yi, Xs, Ys, sf::Color::Black), states);

        // Turn 1 for leaving the parking spot
        target.draw(ARC(C[1].x, C[1].y, Rwmin, 90.0_deg, theta_E[0], sf::Color::Blue), states);
        target.draw(Arrow(C[1].x, C[1].y, Xt, Yt, sf::Color::Blue), states);

        // Turn 2 to make the ego car parallel to the road
        target.draw(ARC(C[0].x, C[0].y, Rwmin, 270.0_deg, theta_E[1], sf::Color::Red), states);
        target.draw(Arrow(C[0].x, C[0].y, C[0].x, Yf, sf::Color::Red), states);

        // Draw the path the car will follow.
        // Middle of the rear axle of the ego car when iterating for leaving the
        // parking spot.
        target.draw(Circle(Xi, Yi, 0.01_m, sf::Color::Green, 8u), states);
        target.draw(Circle(Xs, Ys, 0.01_m, sf::Color::Green, 8u), states);
        target.draw(Circle(Xt, Yt, 0.01_m, sf::Color::Green, 8u), states);
        target.draw(Circle(Xf, Yf, 0.01_m, sf::Color::Green, 8u), states);
    }
    else
    {
        size_t i = m_maneuvers - 1u;

        // Drive to initial position
        target.draw(Arrow(Xi, Yi, Xs, Ys, sf::Color::Black), states);

        // Two last final turns for leaving the parking spot and make the ego
        // car parallel to the road.
        target.draw(ARC(C[i].x, C[i].y, Rwmin, 90.0_deg, theta_E[i], sf::Color::Blue), states);
        target.draw(Arrow(C[i].x, C[i].y, Xt, Yt, sf::Color::Blue), states);
        i--;

        target.draw(ARC(C[i].x, C[i].y, Rwmin, 270.0_deg + theta_sum[i-1], theta_E[i], sf::Color::Red), states);
        target.draw(Arrow(C[i].x, C[i].y, Em[i].x, Em[i].y, sf::Color::Red), states);

        while (i--)
        {
            // Drive backward while turning right
            target.draw(ARC(C[i].x, C[i].y, Rwmin, 90.0_deg + theta_sum[i-1], theta_E[i], sf::Color::Magenta), states);
            target.draw(Arrow(C[i].x, C[i].y, Em[i].x, Em[i].y, sf::Color::Magenta), states);
            i--;

            // Drive forward while turning left
            target.draw(ARC(C[i].x, C[i].y, Rwmin, 270.0_deg, theta_E[i], sf::Color::Cyan), states);
            //target.draw(Arc(C[i].x, C[i].y, Rwmin, 270.0f, RAD2DEG(theta_t[i]), sf::Color::Cyan), states);
            target.draw(Arrow(C[i].x, C[i].y, Em[i].x, Em[i].y, sf::Color::Cyan), states);
        }

        // Draw the path the car will follow.
        // Middle of the rear axle of the ego car when iterating for leaving the
        // parking spot.
        target.draw(Circle(Xi, Yi, 0.01_m, sf::Color::Green, 8u), states);
        target.draw(Circle(Xs, Ys, 0.01_m, sf::Color::Green, 8u), states);
        target.draw(Circle(Xt, Yt, 0.01_m, sf::Color::Green, 8u), states);
        target.draw(Circle(Xf, Yf, 0.01_m, sf::Color::Red, 8u), states);
        target.draw(Circle(Em[0].x, Em[0].y, 0.01_m, sf::Color::Green, 8u), states);
        target.draw(Circle(Em[1].x, Em[1].y, 0.01_m, sf::Color::Green, 8u), states);
        target.draw(Circle(Em[2].x, Em[2].y, 0.01_m, sf::Color::Green, 8u), states);
    }

    #undef ARC
}
