// 2021 -- 2022 Quentin Quadrat quentin.quadrat@gmail.com
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <https://unlicense.org>

#ifndef TURNING_RADIUS_HPP
#  define TURNING_RADIUS_HPP

#  include "Vehicle/VehicleBluePrint.hpp"

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
    TurningRadius(CarBluePrint const& dim, float const steering)
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
