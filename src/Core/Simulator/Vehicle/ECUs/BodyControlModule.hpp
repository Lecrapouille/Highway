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
#  include "Core/Simulator/Vehicle/ECUs/IndicatorStalk.hpp"

// *************************************************************************
//! \brief A BCM is an ECU monitoring and controlling a wide variety of car
//! body, security and convenience functions. In our case, we only simulate
//! bulbs management (indicator bulbs, beam bulbs, and brake bulbs).
// *************************************************************************
class BodyControlModule: public ECU
{
public:

    explicit BodyControlModule(IndicatorStalk const& stalk, double const& pedal_brake,
        size_t const& selected_gear)
        : m_stalk(stalk), m_pedal_brake(pedal_brake), m_selected_gear(selected_gear)
    {}

    void setFlashInterval(Second interval) { m_flash_interval = interval; }
    bool const& isLeftIndicatorBulbOn() const { return m_left_bulb_on; }
    bool const& isRightIndicatorBulbOn() const { return m_right_bulb_on; }
    bool const& isLowBeamBulbOn() const { return m_low_beam_bulb_on; }
    bool const& isHighBeamBulbOn() const { return m_high_beam_bulb_on; }
    bool const& isBrakeLightBulbOn() const { return m_brake_bulb_on; }
    bool const& isRearLightBulbOn() const { return m_rear_bulb_on; }
    bool const& isReverseLightBulbOn() const { return m_reverse_bulb_on; }

private:

    virtual void update(Second const dt) override
    {
        updateRearBulbs();
        updateBeamBulbs();
        updateTurningBulbs(dt);
    }

    void updateRearBulbs()
    {
        m_reverse_bulb_on = (m_selected_gear == 0);
        m_brake_bulb_on = (m_pedal_brake > 0.001);
    }

    void updateBeamBulbs()
    {
        const IndicatorStalk::HeadlightState state = m_stalk.getBeamState();
        switch (state)
        {
            case IndicatorStalk::HeadlightState::LIGHTS_OFF:
                m_low_beam_bulb_on = false;
                m_high_beam_bulb_on = false;
                m_rear_bulb_on = false;
                break;
            case IndicatorStalk::HeadlightState::LOW_BEAM:
                m_low_beam_bulb_on = true;
                m_high_beam_bulb_on = false;
                m_rear_bulb_on = true;
                break;
            case IndicatorStalk::HeadlightState::HIGH_BEAM:
                m_low_beam_bulb_on = false;
                m_high_beam_bulb_on = true;
                m_rear_bulb_on = true;
                break;
        }
    }

    void updateTurningBulbs(Second const dt)
    {
        const IndicatorStalk::IndicatorState state =
            m_stalk.getTurningIndicatorState();
        if (state == IndicatorStalk::IndicatorState::OFF)
        {
            m_left_bulb_on = false;
            m_right_bulb_on = false;
            m_time_accumulator = 0.0_s;
            return ;
        }

        m_time_accumulator += dt;
        if (m_time_accumulator < m_flash_interval)
            return ;

        // Toggle left or right indicator bulbs based on current state
        if (state == IndicatorStalk::IndicatorState::LEFT)
        {
            m_left_bulb_on = !m_left_bulb_on;
            m_right_bulb_on = false;
        }
        else if (state == IndicatorStalk::IndicatorState::RIGHT)
        {
            m_right_bulb_on = !m_right_bulb_on;
            m_left_bulb_on = false;
        }
        else if (state == IndicatorStalk::IndicatorState::WARNING)
        {
            // In warning mode, both bulbs flash together
            m_left_bulb_on = !m_left_bulb_on;
            m_right_bulb_on = !m_right_bulb_on;
        }

        m_time_accumulator = 0.0_s;
    }

private:
    //! \brief Input: indicator stalks
    IndicatorStalk const& m_stalk;
    //! \brief Input: pedal brake for the brake bulb.
    double const& m_pedal_brake;
    //! \brief Input: Currently selected gear.
    size_t const& m_selected_gear;
    //! \brief State of left indicator bulb
    bool m_left_bulb_on = false;
    //! \brief State of right indicator bulb
    bool m_right_bulb_on = false;
    //! \brief State of low beam bulb
    bool m_low_beam_bulb_on = false;
    //! \brief State of high beam bulb
    bool m_high_beam_bulb_on = false;
    //! \brief State of the brake bulb
    bool m_brake_bulb_on = false;
    //! \brief State of the rear bulb
    bool m_rear_bulb_on = false;
    //! \brief State of the reverse bulb
    bool m_reverse_bulb_on = false;
    // Flashing interval in seconds
    Second m_flash_interval = 0.5_s;
    // Time accumulated for flashing
    Second m_time_accumulator = 0.0_s;
};