// 2021 Quentin Quadrat quentin.quadrat@gmail.com
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

#ifndef UTILS_HPP
#  define UTILS_HPP

#  include <SFML/System/Vector2.hpp>
#  include <SFML/System/Vector3.hpp>
#  include <cmath>

#  define ZOOM 0.015f

constexpr float RAD2DEG(float const r) { return r * 57.295779513f; }
constexpr float DEG2RAD(float const d) { return d * 0.01745329251994f; }

constexpr float DISTANCE(float const xa, float const ya, float const xb, float const yb)
{
   return sqrtf((xb - xa) * (xb - xa) + (yb - ya) * (yb - ya));
}

constexpr float DISTANCE(sf::Vector2f const& a, sf::Vector2f const& b)
{
   return sqrtf((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

constexpr float ARC_LENGTH(float const angle, float const radius)
{
   return angle * radius;
}

inline const sf::Vector2f HEADING(sf::Vector2f const& p, float const a)
{
   return sf::Vector2f(cosf(a) * p.x - sinf(a) * p.y,
                       sinf(a) * p.x + cosf(a) * p.y);
}

inline const sf::Vector3f HEADING3F(sf::Vector2f const& p, float const a)
{
   return sf::Vector3f(cosf(a) * p.x - sinf(a) * p.y,
                       sinf(a) * p.x + cosf(a) * p.y,
                       0.0f);
}

#endif
