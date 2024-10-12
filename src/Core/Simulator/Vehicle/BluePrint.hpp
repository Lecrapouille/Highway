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
#  include <vector>

namespace wheel {

// *************************************************************************
//! \brief Wheel blueprint.
// *************************************************************************
struct BluePrint
{
    //! \brief Relative position from the car shape position (middle rear axle).
    sf::Vector2<Meter> offset;
    //! \brief Rayon roue [meter].
    Meter radius;
    //! \brief Thickness [meter] (only used for the rendering).
    Meter thickness;
};

} // namespace wheel
namespace lights {

// *************************************************************************
//! \brief Turning indicator and light blueprint.
// *************************************************************************
struct BluePrint
{
    //! \brief Relative position from the car shape position (middle rear axle)
    sf::Vector2<Meter> offset;
};

} // namespace lights

namespace sensor {

// *************************************************************************
//! \brief Turning indicator and light blueprint.
// *************************************************************************
struct BluePrint
{
    //! \brief Relative position from the car shape position (middle rear axle)
    sf::Vector2<Meter> offset;
    //! \brief Relative relative orientation [degree].
    Degree orientation;
};

} // namespace sensor

namespace vehicle {

// *************************************************************************
//! \brief Car blueprint. See picture [doc/pics/VehicleBlueprint.jpg]
// *************************************************************************
struct BluePrint
{
    //----------------------------------------------------------------------
    //! \brief Wheel's names: FL: front left, FR: front right, RR: rear right,
    //! RL: rear left.
    //----------------------------------------------------------------------
    enum Where { RR, RL, FL, FR, MAX };

    BluePrint() = default;

    //----------------------------------------------------------------------
    //! \brief Define vehicle constants.
    //! See [doc/pics/CarDefinition.png]
    //! \param[in] p_length: car length [meter]
    //! \param[in] p_width: car width [meter]
    //! \param[in] p_wheelbase: wheelbase length [meter]
    //! \param[in] p_back_overhang: back overhang [meter]
    //! \param[in] p_wheel_radius: wheel radius [meter]
    //! \param[in] p_wheel_thickness: wheel thickness [meter]
    //! \param[in] p_turning_diameter: turning diameter [meter]
    //! \param[in] p_steering_ratio: Ratio for the steering wheel [no unit]
    //----------------------------------------------------------------------
    BluePrint(const Meter p_length, const Meter p_width,
              const Meter p_wheelbase, const Meter p_back_overhang,
              const Meter p_wheel_radius, const Meter p_wheel_thickness,
              const Meter p_turning_diameter, const double p_steering_ratio);

    //----------------------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, BluePrint const& bp)
    {
        os << "BluePrint{ length: " << bp.length << ", width: " << bp.width
           << ", track: " << bp.track << ", wheelbase: " << bp.wheelbase
           << ", back overhang: " << bp.back_overhang << ", front_overhang: "
           << bp.front_overhang << ", max steering angle: " << bp.max_steering_angle
           << " }";
        return os;
    }

    //! \brief Vehicle length [meter].
    Meter length;
    //! \brief Vehicle width [meter].
    Meter width;
    //! \brief Wheel to wheel distance along width [meter].
    Meter track;
    //! \brief Wheel to wheel distance along the length [meter].
    Meter wheelbase;
    //! \brief Rear overhand [meter] (fr: Porte à faux arrière).
    Meter back_overhang;
    //! \brief Front overhang [meter] (fr: Porte à faux avant).
    Meter front_overhang;
    //! \brief Limit of control.outputs.steering angle absolute angle [rad].
    Radian max_steering_angle;
    //! \brief Ratio of the angle of the steering wheel to the angle of the wheels.
    double steering_ratio;
    //! \brief Blue prints for the wheels.
    std::vector<wheel::BluePrint> wheels;
    //! \brief Blue prints for the turning indicators.
    std::vector<lights::BluePrint> turning_indicators;
    //! \brief Blue prints for the vehicle lights.
    std::vector<lights::BluePrint> lights;
};

} // namespace vehicle