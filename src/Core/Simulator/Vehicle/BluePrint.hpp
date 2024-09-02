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
//#  include <SFML/Graphics.hpp>
#  include <array>

namespace vehicle {
namespace wheel {

// *************************************************************************
//! \brief Wheel blueprint.
// *************************************************************************
struct BluePrint
{
    //! \brief Relative position from the car shape position (middle rear axle)
    sf::Vector2<Meter> offset;
    //! \brief current position and altitude inside the world coordinate.
    sf::Vector2<Meter> position; // FIXME to be moved outside + 3D
    //! \brief Rayon roue [meter]
    Meter radius;
    //! \brief Epaisseur roue [meter] (only used for the rendering)
    Meter width;
};

} // namespace wheel

// *************************************************************************
//! \brief Car blueprint. See picture ../../doc/pics/CarDefinition.png
// *************************************************************************
struct BluePrint
{
    //--------------------------------------------------------------------------
    //! \brief Wheel's names: FL: front left, FR: front rigeht, RR: rear right,
    //! RL: rear left.
    //--------------------------------------------------------------------------
    enum Where { RR, RL, FL, FR, MAX };
    BluePrint() = default;
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
    BluePrint(const Meter l, const Meter w, const Meter wb, const Meter bo,
              const Meter wr, const Meter td);

    friend std::ostream& operator<<(std::ostream& os, BluePrint const& bp)
    {
        os << "BluePrint{ length: " << bp.length << ", width: " << bp.width
           << ", track: " << bp.track << ", wheelbase: " << bp.wheelbase
           << ", back overhang: " << bp.back_overhang << ", front_overhang: "
           << bp.front_overhang << ", max steering angle: " << bp.max_steering_angle
           << " }";
        return os;
    }

    //! \brief Vehicle length [meter]
    Meter length;
    //! \brief Vehicle width [meter]
    Meter width;
    //! \brief Wheel to wheel distance along width [meter]
    Meter track;
    //! \brief Wheel to wheel distance along the length [meter]
    Meter wheelbase;
    //! \brief Porte a faux arriere [meter]
    Meter back_overhang;
    //! \brief Porte a faux avant [meter]
    Meter front_overhang;
    //! \brief Limit of control.outputs.steering angle absolute angle [rad]
    Radian max_steering_angle;
    //! \brief Blue prints for the wheels
    std::array<wheel::BluePrint, BluePrint::Where::MAX> wheels;
    //! \brief Blue prints for the turning indicators
    //std::array<LightsBluePrint, CarBluePrint::Where::MAX> turning_indicators;
    //! \brief Blue prints for the vehicle lights
    //std::array<LightsBluePrint, CarBluePrint::Where::MAX> lights;
};

} // namespace vehicle