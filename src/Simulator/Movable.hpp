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
#ifndef MOVABLE_HPP
#  define MOVABLE_HPP

#  include <SFML/System/Vector2.hpp>

// *****************************************************************************
//! \brief Base class for computing physic.
// *****************************************************************************
class Movable // TODO faire NonMovable qui retourne speed et acc = 0
{
public:

    //--------------------------------------------------------------------------
    //! \brief Needed beause of virtual methods.
    //--------------------------------------------------------------------------
    virtual ~Movable() = default;

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
    virtual void init(float acceleration, float speed, sf::Vector2f position, float heading)
    {
        m_acceleration = acceleration;
        m_speed = speed;
        m_position = position;
        m_heading = heading;
    }

    //--------------------------------------------------------------------------
    //! \brief Update discrete time equations from continuous time equations.
    //--------------------------------------------------------------------------
    virtual void update(float const dt) = 0;

    //--------------------------------------------------------------------------
    //! \brief Const getter: return longitudinal acceleration [meter/second^2].
    //--------------------------------------------------------------------------
    inline float acceleration() const
    {
        return m_acceleration;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the longitudinal speed [meter/second].
    //--------------------------------------------------------------------------
    inline float speed() const
    {
        return m_speed;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the position of the middle of the rear axle
    //! inside the world coordinates.
    //--------------------------------------------------------------------------
    inline sf::Vector2f position() const
    {
        return m_position;
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading (yaw angle) [rad].
    //--------------------------------------------------------------------------
    inline float heading() const
    {
        return m_heading;
    }

protected:

    //! \brief Longitudinal acceleration [meter / second / second].
    float m_acceleration;
    //! \brief Longitudinal speed [meter / second].
    float m_speed;
    //! \brief World position of the car (its center of the rear axle) [meter].
    sf::Vector2f m_position;
    //! \brief Yaw angle of the vehicle [radian].
    float m_heading;
};

#endif
