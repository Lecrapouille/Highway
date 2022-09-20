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

#include "SelfParking/Trajectories/Trajectory.hpp"
#include "SelfParking/Trajectories/DiagonalTrajectory.hpp"
#include "SelfParking/Trajectories/PerpendicularTrajectory.hpp"
#include "SelfParking/Trajectories/ParallelTrajectory.hpp"
#include "Vehicle/VehicleControl.hpp"
#include <iostream>

//------------------------------------------------------------------------------
bool CarTrajectory::update(CarControl& control, float const dt)
{
    m_time += dt;

    if (USE_KINEMATIC)
    {
        control.set_speed(m_speeds.get(m_time));
        control.set_steering(m_steerings.get(m_time));

        return !m_speeds.end(m_time);
    }
    else
    {
        control.set_acceleration(m_accelerations.get(m_time), dt);
        control.set_steering_rate(m_steerings.get(m_time), dt);

        return !m_accelerations.end(m_time);
    }
}

//------------------------------------------------------------------------------
// FIXME a supprimer
CarTrajectory::Ptr CarTrajectory::create(Parking::Type const type)
{
    switch (type)
    {
    case Parking::Type::Parallel:
        return std::make_unique<ParallelTrajectory>();
    case Parking::Type::Perpendicular:
        return std::make_unique<PerpTrajectory>();
    default:
        return std::make_unique<DiagonalTrajectory>();
    }
}
