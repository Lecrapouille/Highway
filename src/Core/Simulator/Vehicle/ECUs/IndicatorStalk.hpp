//==============================================================================
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
//==============================================================================

#pragma once

#  include "Core/Simulator/Vehicle/ECUs/ECU.hpp"

// *************************************************************************
//! \brief Class representing the indicator stalk (or steering column control)
//! typically found in vehicles, which manages various driver-controlled functions
//! such as turn signals, headlights, hazard lights, and wipers.
//! [doc/pics/IndicatorStalk.jpeg]
// *************************************************************************
class IndicatorStalk
{
public:

    //! \brief Enum for indicator states (turn signals).
    enum class IndicatorState
    {
        OFF,
        LEFT,
        RIGHT,
        WARNING
    };

    //! \brief Enum for headlight states.
    enum class HeadlightState
    {
        LIGHTS_OFF,
        //! [doc/pics/LowBeam.png]
        LOW_BEAM,
        //! [doc/pics/HighBeam.png]
        HIGH_BEAM
    };

public:

    //! \brief Method to turn off indicators.
    void setTurnIndicatorOff()
    {
        if (m_indicator_state != IndicatorState::WARNING)
        {
            m_indicator_state = IndicatorState::OFF;
        }
    }

    //! \brief Method to activate left turn signal.
    void setTurnIndicatorLeft()
    {
        if (m_indicator_state != IndicatorState::WARNING)
        {
            m_indicator_state = IndicatorState::LEFT;
        }
    }

    //! \brief Method to activate right turn signal.
    void setTurnIndicatorRight()
    {
        if (m_indicator_state != IndicatorState::WARNING)
        {
            m_indicator_state = IndicatorState::RIGHT;
        }
    }

    //! \brief Method to activate hazard lights (warning mode).
    void switchOnWarning()
    {
        if (m_indicator_state != IndicatorState::WARNING)
        {
            m_indicator_state = IndicatorState::WARNING;
        }
    }

    //! \brief Method to deactivate hazard lights (warning mode).
    void switchOffWarning()
    {
        if (m_indicator_state == IndicatorState::WARNING)
        {
            m_indicator_state = IndicatorState::OFF;
        }
    }

    IndicatorState getTurningIndicatorState() const
    {
        return m_indicator_state;
    }

public: // Methods for headlights

    void setLightsOff()
    {
        m_headlight_state = HeadlightState::LIGHTS_OFF;
    }

    void setLowBeamOn()
    {
        m_headlight_state = HeadlightState::LOW_BEAM;
    }

    void setHighBeamOn()
    {
        m_headlight_state = HeadlightState::HIGH_BEAM;
    }

    HeadlightState getBeamState() const
    {
        return m_headlight_state;
    }

private:

    //! \brief Current state of indicators.
    IndicatorState m_indicator_state = IndicatorState::OFF;
    //! \brief Current state of headlights.
    HeadlightState m_headlight_state = HeadlightState::LIGHTS_OFF;
};