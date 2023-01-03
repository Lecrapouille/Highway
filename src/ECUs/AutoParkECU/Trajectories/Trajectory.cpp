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

#include "ECUs/AutoParkECU/Trajectories/Trajectory.hpp"
#include "ECUs/AutoParkECU/Trajectories/ParallelTrajectory.hpp"
//#include "SelfParking/Trajectories/DiagonalTrajectory.hpp"
//#include "SelfParking/Trajectories/PerpendicularTrajectory.hpp"
#include "Vehicle/Car.hpp"

//------------------------------------------------------------------------------
bool CarTrajectory::update(Car& car, Second const dt)
{
    m_time += dt;

    if (USE_KINEMATIC)
    {
        car.refSpeed(m_speeds.get(m_time));
        car.refSteering(m_steerings.get(m_time));

        return !m_speeds.end(m_time);
    }
    else
    {
        std::cerr << "Not yet implemented" << std::endl;
        //car.refAcceleration(m_accelerations.get(t), dt);
        //control.refSteeringRate(m_steerings.get(t), dt);

        return !m_accelerations.end(m_time);
    }
}

//------------------------------------------------------------------------------
CarTrajectory::Ptr CarTrajectory::create(Parking::Type const type)
{
    switch (type)
    {
    case Parking::Type::Parallel:
        return std::make_unique<ParallelTrajectory>();
    case Parking::Type::Perpendicular:
        // FIXME https://github.com/Lecrapouille/Highway/issues/33
        // return std::make_unique<PerpTrajectory>();
        assert(false && "Not yet implemented");
        break;
    case Parking::Type::Diagonal45:
    case Parking::Type::Diagonal60:
    case Parking::Type::Diagonal75:
        // FIXME https://github.com/Lecrapouille/Highway/issues/33
        // return std::make_unique<DiagonalTrajectory>();
        assert(false && "Not yet implemented");
        break;
    default:
        assert(false && "Not yet implemented");
        break;
    }
}
