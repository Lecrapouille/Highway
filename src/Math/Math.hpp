//==============================================================================
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
//==============================================================================

#ifndef MATH_UTILS_HPP
#  define MATH_UTILS_HPP

#  include "Math/Units.hpp"
#  include <SFML/System/Vector2.hpp>
#  include <SFML/System/Vector3.hpp>
#  include <cmath>

namespace math {

//------------------------------------------------------------------------------
//! \brief Linear interpolation between \c from and \c to.
//! \param[in] weight is clamped to the range [0.0, 1.0].
//! \param[in] from initial value (when weight == 0.0).
//! \param[in] to final value (when weight == 1.0).
//! \tparam T type for \c from and \c to (i.e. float, double, Meter ...).
//! \tparam U type for \c weight (i.e. float or double).
//! \return the ponderation value.
//! \note for angle use instead \c lerp_angle
//------------------------------------------------------------------------------
template<typename T, typename U=T>
inline T lerp(T const from, T const to, U const weight)
{
   return from + (to - from) * weight;
}

//------------------------------------------------------------------------------
//! \brief Map a value from an initial [start1 stop1] range to a final range
//! [start2 stop2].
//! \param[in] value the value inside the initial [start1 stop1] to map to the
//! final range [start2 stop2].
//! \return value mapped into [start2 stop2].
//! \note the check if \c value is correctly bounded inside [start1 stop1] is
//! not made.
//------------------------------------------------------------------------------
template<class T>
inline T map(T const value, T const start1, T const stop1, T const start2, T const stop2)
{
   return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

//------------------------------------------------------------------------------
//! \brief Constrain \c value inside the range [lower upper].
//------------------------------------------------------------------------------
template<typename T>
inline T constrain(T const value, T const lower, T const upper)
{
    if (value <= lower) { return lower; }
    if (value >= upper) { return upper; }
    return value;
}

//------------------------------------------------------------------------------
//! \brief Lerping angle. Same as \c lerp but makes sure the values interpolate
//! correctly when they wrap around 360 degrees.
//!
//! \param[in] weight is clamped to the range [0, 1].
//! \param[in] from starting angle [degree].
//! \param[in] to ending angle [degree].
//!
//! \note Using Unity algorithm
//! https://gist.github.com/shaunlebron/8832585?permalink_comment_id=3227412#gistcomment-3227412
//------------------------------------------------------------------------------
inline Degree lerp_angle(Degree const from, Degree const to, double weight)
{
   // I dislike https://github.com/godotengine/godot/issues/30564
   //   constexpr double TAU = Radian(360.0_deg).value();
   //   const double difference = std::fmod((to - from).value(), TAU);
   //   const double distance = std::fmod(2.0 * difference, TAU) - difference;
   //   return Radian(from.value() + distance * weight);
   // because:
   //   lerp_angle(-90.0_deg, 90.0_deg, 0.0) return 90.0_deg
   //   lerp_angle(-90.0_deg, 90.0_deg, 0.5) return -180.0_deg instead of 0.0_deg
   //   lerp_angle(-90.0_deg, 90.0_deg, 1.0) return -270.0_deg instead of -90.0_deg

   auto repeat = [](Degree const t, Degree const m) -> Degree
   {
      return constrain(t - units::math::floor(t / m) * m, 0.0_deg, m);
   };
   const Degree dt = repeat(to - from, 360.0_deg);
   return lerp(from, from + (dt > 180.0_deg ? dt - 360.0_deg : dt), weight);
}

//------------------------------------------------------------------------------
//! \brief Return the length in Meter of two given positions.
//------------------------------------------------------------------------------
inline Meter distance(sf::Vector2<Meter> const& a, sf::Vector2<Meter> const& b)
{
   return units::math::sqrt(units::math::pow<2>(b.x - a.x) + units::math::pow<2>(b.y - a.y));
}

//------------------------------------------------------------------------------
//! \brief Return the length in Meter of two given positions.
//------------------------------------------------------------------------------
inline float distance(sf::Vector2f const& a, sf::Vector2f const& b)
{
   return sqrtf((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

//------------------------------------------------------------------------------
//! \brief Return the shorter angle between two vectors.
//------------------------------------------------------------------------------
inline Radian orientation(sf::Vector2<Meter> const& a, sf::Vector2<Meter> const& b)
{
   const auto teta = (b.y - a.y) / (b.x - a.x);
   Radian angle(units::math::atan(teta));
   if (b.x < a.x)
      angle += 180.0_deg;
   else if (b.y < a.y)
      angle += 360.0_deg;
   return angle;
}

//------------------------------------------------------------------------------
//! \brief Return the length of an arc.
//! \param[in] angle [radian].
//! \param[in] radius [meter].
//! \return the length [meter].
//------------------------------------------------------------------------------
constexpr Meter arc_length(Radian const angle, Meter const radius)
{
   return angle.value() * radius;
}

//------------------------------------------------------------------------------
//! \brief Compute the heading rotation of a given point.
//! \param[in] p the position of the point to rotate [meter].
//! \param[in] a the rotation angle [radian].
//! \return the new position of \p [meter].
//------------------------------------------------------------------------------
inline sf::Vector2<Meter> heading(sf::Vector2<Meter> const& p, Radian const a)
{
   return sf::Vector2<Meter>(units::math::cos(a) * p.x - units::math::sin(a) * p.y,
                             units::math::sin(a) * p.x + units::math::cos(a) * p.y);
}

} // namespace math

#endif // MATH_UTILS_HPP
