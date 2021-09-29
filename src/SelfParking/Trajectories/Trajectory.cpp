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
        control.set_steering_speed(m_steerings.get(m_time), dt);

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
