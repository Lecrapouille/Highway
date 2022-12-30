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

#  include "Math/Units.hpp"
#  include <SFML/System/Vector2.hpp>
#  include <SFML/System/Vector3.hpp>
#  include <cmath>

//constexpr float RAD2DEG(float const r) { return r * 57.295779513f; }
//constexpr float DEG2RAD(float const d) { return d * 0.01745329251994f; }
template<class T>
constexpr T POW2(T const x)  { return x * x; }

template<typename T>
inline T constrain(T const value, T const lower, T const upper)
{
    if (value < lower)
        return lower;

    if (value > upper)
        return upper;

    return value;
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
