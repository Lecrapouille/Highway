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
#ifndef VEHICLE_BLUEPRINT_HPP
#  define VEHICLE_BLUEPRINT_HPP

#  include <SFML/Graphics.hpp>
#  include <array>

// *************************************************************************
//! \brief Wheel blueprint.
// *************************************************************************
struct WheelBluePrint
{
    //! \brief Relative position from the car shape position (middle rear axle)
    sf::Vector2f offset;
    //! \brief current position and altitude inside the world coordinate.
    sf::Vector2f position; // FIXME to be moved outside + 3D
    //! \brief Rayon roue [meter]
    float radius;
    //! \brief Epaisseur roue [meter] (only used for the rendering)
    float width;
};

// *************************************************************************
//! \brief Car blueprint. See picture ../../doc/pics/CarDefinition.png
// *************************************************************************
struct CarBluePrint
{
    //--------------------------------------------------------------------------
    //! \brief Wheel's names: FL: front left, FR: front rigeht, RR: rear right,
    //! RL: rear left.
    //--------------------------------------------------------------------------
    enum WheelName { RR, RL, FL, FR, MAX };

    //----------------------------------------------------------------------
    //! \brief Define vehicle constants:
    //! See ../../doc/pics/CarDefinition.png
    //! \param[in] l: car length [meter]
    //! \param[in] w: car width [meter]
    //! \param[in] wb: wheelbase length [meter]
    //! \param[in] bo: back overhang [meter]
    //! \param[in] wr: wheel radius [meter]
    //! \param[in] tc: turning diameter [meter]
    //----------------------------------------------------------------------
    CarBluePrint(const float l, const float w, const float wb, const float bo,
                 const float wr, const float td);

    //! \brief Vehicle length [meter]
    float length;
    //! \brief Vehicle width [meter]
    float width;
    //! \brief Wheel to wheel distance along width [meter]
    float track;
    //! \brief Wheel to wheel distance along the length [meter]
    float wheelbase;
    //! \brief Porte a faux arriere [meter]
    float back_overhang;
    //! \brief Porte a faux avant [meter]
    float front_overhang;
    //! \brief Limit of control.outputs.steering angle absolute angle [rad]
    float max_steering_angle;
    //! \brief Blue prints for the wheels
    std::array<WheelBluePrint, WheelName::MAX> wheels;
};

// *****************************************************************************
//! \brief Trailer blueprint
// *****************************************************************************
struct TrailerBluePrint
{
    //--------------------------------------------------------------------------
    //! \brief Wheel's names: RR: rear right, RL: rear left.
    //--------------------------------------------------------------------------
    enum WheelName { RR, RL, MAX };

    //--------------------------------------------------------------------------
    //! \brief Define trailer constants
    //! \param[in] l: trailer length [meter]
    //! \param[in] w: trailer width [meter]
    //! \param[in] wb: wheelbase length (fork length) [meter]
    //! \param[in] bo: back overhang [meter]
    //! \param[in] wr: wheel radius [meter]
    //--------------------------------------------------------------------------
    TrailerBluePrint(const float l, const float w, const float d,
                     const float bo, const float wr);

    //! \brief Vehicle length [meter]
    float length;
    //! \brief Vehicle width [meter]
    float width;
    //! \brief Wheel to wheel distance along width [meter]
    float track;
    //! \brief Wheel to wheel distance along the length [meter]
    float wheelbase;
    //! \brief Porte a faux arriere [meter]
    float back_overhang;
    //! \brief Width of the fork [meter] (only used for the rendering)
    float fork_width = 0.1f;
    //! \brief Blue prints for the wheels
    std::array<WheelBluePrint, WheelName::MAX> wheels;
};

#endif
