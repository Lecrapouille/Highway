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

#pragma once

#  include "Core/Math/Units.hpp"
#  include <limits>

// *****************************************************************************
//! \brief Base class for computing physic. This class only offers init method
//! and getters. Since this class is not supposed to be used directly, there is
//! no update(dt) method to update acceleration, speed, position ... This is the
//! goal of the parent class.
// *****************************************************************************
class Movable
{
public:

    //--------------------------------------------------------------------------
    //! \brief Const getter: return longitudinal acceleration [meter/second^2].
    //--------------------------------------------------------------------------
    inline MeterPerSecondSquared const& acceleration() const
    {
        return m_acceleration;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the longitudinal speed [meter/second].
    //--------------------------------------------------------------------------
    inline MeterPerSecond const& speed() const
    {
        return m_speed;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the position of the middle of the rear axle
    //! inside the world coordinates.
    //--------------------------------------------------------------------------
    inline sf::Vector2<Meter> const& position() const
    {
        return m_position;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading (yaw angle) [rad].
    //--------------------------------------------------------------------------
    inline Radian const& heading() const
    {
        return m_heading;
    }

protected:

    //--------------------------------------------------------------------------
    //! \brief Set initial state values needed by physical equations.
    //! \param[in] position: the (x, y) world coordinated of the car (can be the
    //!   middle of the rear axle).
    //! \param[in] acceleration: initial longitudinal acceleration [meter /
    //!   second / second].
    //! \param[in] speed: initial longitudinal speed [meter / second].
    //! \param[in] position: initial world position of the car (its center of
    //!   the rear axle) [meter].
    //! \param[in] heading: the initial yaw angle of the vehicle [radian].
    //--------------------------------------------------------------------------
    void init(MeterPerSecondSquared const acceleration, MeterPerSecond const speed,
              sf::Vector2<Meter> const position, Radian const heading)
    {
        m_acceleration = acceleration;
        m_speed = speed;
        m_position = position;
        m_heading = heading;
    }

protected:

    //! \brief Longitudinal acceleration. Units: [meter / second / second].
    MeterPerSecondSquared m_acceleration{math::nan<double>()};
    //! \brief Longitudinal speed. Units: [meter / second].
    MeterPerSecond m_speed{math::nan<double>()};
    //! \brief World position of the car (its center of the rear axle). Units: [meter].
    sf::Vector2<Meter> m_position{Meter(math::nan<double>()), Meter(math::nan<double>())};
    //! \brief Yaw angle of the vehicle. Units: [radian].
    Radian m_heading{math::nan<double>()};
};