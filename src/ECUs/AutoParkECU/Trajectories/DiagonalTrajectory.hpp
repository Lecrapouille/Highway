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

#ifndef DIAGONAL_TRAJECTORY_HPP
#  define DIAGONAL_TRAJECTORY_HPP

#  include "SelfParking/Trajectories/Trajectory.hpp"

// *************************************************************************
//! \brief
// *************************************************************************
class DiagonalTrajectory: public CarTrajectory
{
public:

    virtual bool init(Car& car, Parking const& parking, bool const entering) override;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:

    bool computePathPlanning(Car const& car, Parking const& parking, bool const entering);
    void generateReferenceTrajectory(Car const& car, bool const entering, float const vmax, float const ades);

private:

    // rayon pour sortir
    float Rin1;
    float theta1;
    float beta1; // angle braquage

    float Rin2;
    float theta2;
    float beta2; // angle braquage

    float Xi, Yi, Xdm, Ydm, Xc, Yc, dl;
};

#endif
