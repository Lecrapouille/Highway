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

#  include <SFML/System/Vector2.hpp>
#  include <SFML/System/Vector3.hpp>
#  include <cmath>

constexpr float RAD2DEG(float const r) { return r * 57.295779513f; }
constexpr float DEG2RAD(float const d) { return d * 0.01745329251994f; }
constexpr float POW2(float const x)  { return x * x; }

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

constexpr float ARC_LENGTH(float const angle, float const radius) // [rad] * [m]
{
   return angle * radius;
}

inline sf::Vector2f HEADING(sf::Vector2f const& p, float const a)
{
   return sf::Vector2f(cosf(a) * p.x - sinf(a) * p.y,
                       sinf(a) * p.x + cosf(a) * p.y);
}

inline sf::Vector3f HEADING3F(sf::Vector2f const& p, float const a)
{
   return sf::Vector3f(cosf(a) * p.x - sinf(a) * p.y,
                       sinf(a) * p.x + cosf(a) * p.y,
                       0.0f);
}

#endif
