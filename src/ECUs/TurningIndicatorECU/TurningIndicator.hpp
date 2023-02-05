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

#ifndef TURNING_INDICATOR_ECU_HPP
#  define TURNING_INDICATOR_ECU_HPP

#  include "Vehicle/ECU.hpp"

enum TurningIndicator { Off, Left, Right, Warnings };

// ****************************************************************************
//! \brief
// ****************************************************************************
class TurningIndicatorECU : public ECU
{
public:

    TurningIndicatorECU(Second const blinking = 0.25_s)
        : m_blinking(blinking)
    {}

    //-------------------------------------------------------------------------
    //! \brief Emulate the driver pushing up the turning indicator (button
    //! pressed).
    //-------------------------------------------------------------------------
    void up()
    {
        m_state = (m_state == TurningIndicator::Right)
                ? TurningIndicator::Off
                : TurningIndicator::Left;
    }

    //-------------------------------------------------------------------------
    //! \brief Emulate the driver pushing up the turning indicator(button
    //! pressed).
    //-------------------------------------------------------------------------
    void down()
    {
        m_state = (m_state == TurningIndicator::Left)
                ? TurningIndicator::Off
                : TurningIndicator::Right;
    }

    //-------------------------------------------------------------------------
    //! \brief Emulate the driver pushing up the warnings button.
    //-------------------------------------------------------------------------
    void warnings(bool const enable)
    {
        m_warnings = enable;
    }

    //-------------------------------------------------------------------------
    //! \brief Is the car turning left, rigth, warning or not flashing.
    //-------------------------------------------------------------------------
    TurningIndicator state() const
    {
        if (m_warnings)
            return TurningIndicator::Warnings;

        return m_state;
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    void state(TurningIndicator const& state)
    {
        if (state == TurningIndicator::Warnings)
        {
            m_warnings = true;
        }
        else
        {
            m_state = state;
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Is the car turning left, rigth, warning or not flashing.
    //-------------------------------------------------------------------------
    void getLights(bool& left, bool& right) const
    {
        left = m_left_light;
        right = m_right_light;
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    virtual void update(Second const dt) override
    {
        if ((m_state == TurningIndicator::Off) && (m_warnings == false))
        {
            m_left_light = m_right_light = false;
            m_clock.restart();
            return ;
        }

        if (Second(m_clock.getElapsedTime().asSeconds()) < m_blinking)
            return ;

        m_clock.restart();
        m_pwm ^= true;

        // TBD Add a listener to send m_left_light and m_right_light ?
        m_left_light = m_pwm && (m_warnings || (m_state == TurningIndicator::Left));
        m_right_light = m_pwm && (m_warnings || (m_state == TurningIndicator::Right));
    }

public: // Inheritance with \c Component class

    //-------------------------------------------------------------------------
    //! \brief Implement Entity-Component-System to allow adding compositions
    //! dynamically.
    //-------------------------------------------------------------------------
    COMPONENT_CLASSTYPE(TurningIndicatorECU, ECU);

private:

    virtual void onSensorUpdated(Sensor&) override
    {}

protected:

    //! \brief Current turning state.
    TurningIndicator m_state = TurningIndicator::Off;
    //! \brief Are warnings enables ?
    bool m_warnings = false;
    //! \brief Current left ligft state (enable/disable).
    bool m_left_light = false;
    //! \brief Current right ligft state (enable/disable).
    bool m_right_light = false;
    //! \brief Clock needed for blinking lights.
    sf::Clock m_clock;
    //! brief Duration of lights blinking.
    Second m_blinking;
    //! \brief Square pulse enabling/disabling lights.
    bool m_pwm = false;
};

#endif