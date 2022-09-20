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

#ifndef TURNING_RADIUS_HPP
#  define TURNING_RADIUS_HPP

#  include "Vehicle/VehicleDimension.hpp"

// *****************************************************************************
//! \brief The turning radius is the radius of the circle created by a
//! vehicle when it turns with a fixed steering angle. Some formula are given
//! at http://www.autoturn.ch/giration/standard_r.html
//! This structure holds these results.
//! Turning circle:
//! ../../../doc/pics/TurningCircle.png
//! Turning radius of internal and external corner of the ego car doing the
//! parking maneuver:
//! ../../../doc/pics/TurninRadius.png
// *****************************************************************************
struct TurningRadius
{
    //--------------------------------------------------------------------------
    //! \brief Compute all turning radius information needed for doing parking
    //! maneuvers. Equations (in where L means e and p means pf in previous picture)
    //! ../../../doc/pics/TurningRadiusEq.png
    //! \param[in] dim: the vehicle dimension.
    //! \param[in] steering: the desired steerin angle [rad].
    //--------------------------------------------------------------------------
    TurningRadius(CarDimension const& dim, float const steering)
    {
        const float e = dim.wheelbase;
        const float w = dim.width;
        const float p = dim.front_overhang;

        // R: turning radius of the fake front wheel [m]
        middle = e / sinf(steering);

        // Ri: Inner turning radius [m]
        internal = sqrtf(middle * middle - e * e) - (w / 2.0f);

        // Re: External (outer) turning radius [m]
        external = sqrtf((internal + w) * (internal + w) + (e + p) * (e + p));

        // Largeur balayee [m]
        bal = external - internal;

        // surlargeur [m]
        sur = bal - w;
    }

    //! \brief turning radius of the virtual wheel located in the middle of the
    //! front axle [meter].
    float middle;
    //! \brief turning radius of the internal wheel [meter].
    float internal;
    //! \brief turning radius of the external wheel [meter].
    float external;
    //! \brief Largeur balayee [meter]
    float bal;
    //! \brief Surlargeur [meter]
    float sur;
};

#endif
