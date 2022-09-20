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


#ifndef PARALLEL_TRAJECTORY_HPP
#  define PARALLEL_TRAJECTORY_HPP

#  include "SelfParking/Trajectories/Trajectory.hpp"

// *****************************************************************************
//! \brief Compute the trajectory allowing the car to park. For parallel parking
//! only!
// *****************************************************************************
class ParallelTrajectory: public CarTrajectory
{
public:

    virtual bool init(Car& car, Parking const& parking, bool const entering) override;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:

    //--------------------------------------------------------------------------
    //! \brief Compute the trajectory path when the car needs N maneuvers
    //! Loop:
    //! - Step 1:  ../../../doc/pics/ParallelStep1.png
    //! - Step 2:  ../../../doc/pics/ParallelStep2.png
    //! - Leaving loop condition: ../../../doc/pics/ParallelLeavingCondition.png
    //! - Final Step: ../../../doc/pics/ParallelFinalStep.png
    //! Equations: ../../../doc/pics/ParallelManeuversEq.png
    //--------------------------------------------------------------------------
    size_t computePathNTrials(Car const& car, Parking const& parking);
    size_t computePath1Trial(Car const& car, Parking const& parking);

    void generateReferences(Car const& car, Parking const& parking,
                            float const VMAX, float const ADES);
private:

    //! \brief Minimal turning radius for the internal point of the car.
    float Rimin;
    //! \brief Minimal turning radius for the external point of the car.
    float Remin;
    //! \brief Minimal turning radius for the external point of the car.
    float Rwmin;
    //! \brief Minimal turning radius for the external point of the car.
    float Lmin;
    //! \brief Number needed of maneuvers for parking the car (change of gear)
    size_t m_maneuvers = 0u;
    //! \brief X-Y world coordinates of the middle rear axle of the ego car.
    std::vector<sf::Vector2f> Em;
    //! \brief X-Y world coordinates of the immedite center of rotations.
    std::vector<sf::Vector2f> C;
    float Xs, Ys, Xi, Yi, Xf, Yf, Xt, Yt;
    //! \brief
    std::vector<float> theta_E, theta_t, theta_s, theta_p, theta_g, theta_sum, Rrg;
};

#endif
