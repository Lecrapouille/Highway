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

#ifndef FOO_HPP
#  define FOO_HPP

// FIXME https://github.com/Lecrapouille/Highway/issues/11
// Clean this file

#  include "Math/Units.hpp"
#  include <SFML/System/Vector2.hpp>
#  include <SFML/System/Vector3.hpp>
#  include <cmath>

//-----------------------------------------------------------------------------
template<class T> inline T POW2(T const x) { return x * x; }

//-----------------------------------------------------------------------------
//! \brief Linear interpolation.
//! \param[in] weight is clamped to the range [0, 1].
template<typename T, typename U>
inline T lerp(T const from, T const to, U const weight)
{
   return from + (to - from) * weight;
}

//-----------------------------------------------------------------------------
//! \brief Map a value into a given range.
template<class T>
inline T map(T const value, T const start1, T const stop1, T const start2, T const stop2)
{
   return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

//-----------------------------------------------------------------------------
//! \brief Constrain value: std::min(std::max(a[i], lower), upper)
template<typename T>
inline T constrain(T const value, T const lower, T const upper)
{
    if (value < lower) { return lower; }
    if (value > upper) { return upper; }
    return value;
}

//-----------------------------------------------------------------------------
//! \brief Lerping angle. Same as \c lerp but makes sure the values interpolate
//! correctly when they wrap around 360 degrees.
//!
//! \param[in] weight is clamped to the range [0, 1].
//! \param[in] from starting angle [degree].
//! \param[in] to ending angle [degree].
//!
//! \note Using Unity algorithm
//! https://gist.github.com/shaunlebron/8832585?permalink_comment_id=3227412#gistcomment-3227412
//-----------------------------------------------------------------------------
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

inline float DISTANCE(float const xa, float const ya, float const xb, float const yb)
{
   return sqrtf(POW2(xb - xa) + POW2(yb - ya));
}

inline float DISTANCE(sf::Vector2f const& a, sf::Vector2f const& b)
{
   return sqrtf(POW2(b.x - a.x) + POW2(b.y - a.y));
}

inline Meter DISTANCE(sf::Vector2<Meter> const& a, sf::Vector2<Meter> const& b)
{
   return units::math::sqrt(units::math::pow<2>(b.x - a.x) + units::math::pow<2>(b.y - a.y));
}

inline Radian ORIENTATION(sf::Vector2<Meter> const& a, sf::Vector2<Meter> const& b)
{
   const auto teta = (b.y - a.y) / (b.x - a.x);
   Radian angle(units::math::atan(teta));
   if (b.x < a.x)
      angle += 180.0_deg;
   else if (b.y < a.y)
      angle += 360.0_deg;
   return angle;
}

constexpr Meter ARC_LENGTH(Radian const angle, Meter const radius) // [rad] * [m]
{
   return angle.value() * radius;
}

inline sf::Vector2<Meter> HEADING(sf::Vector2<Meter> const& p, Radian const a)
{
   return sf::Vector2<Meter>(units::math::cos(a) * p.x - units::math::sin(a) * p.y,
                             units::math::sin(a) * p.x + units::math::cos(a) * p.y);
}

inline sf::Vector3f HEADING3F(sf::Vector2f const& p, float const a)
{
   return sf::Vector3f(cosf(a) * p.x - sinf(a) * p.y,
                       sinf(a) * p.x + cosf(a) * p.y,
                       0.0f);
}

#endif
