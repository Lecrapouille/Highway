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
