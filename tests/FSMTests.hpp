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

#ifndef QQ_HPP
#define QQ_HPP

#include "StateMachine.hpp"

enum MotorStateID { IDLE, STARTING, SPINNING, STOPPING, IGNORING_EVENT, CANNOT_HAPPEN, MAX_STATES };

// *****************************************************************************
//! \brief
// *****************************************************************************
class MotorControl : public StateMachine<MotorControl, MotorStateID>
{
public:

    MotorControl();
    void halt();
    void refspeed(int const value);
    inline float refspeed() const
    {
        return m_refspeed;
    }

protected:

    virtual const char* stringify(MotorStateID const state) const override;

private:

    float m_refspeed = 0.0f;
};

#endif
