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

#  include <cmath>

#  define ZOOM 0.015f

#  define RAD2DEG(r) ((r) * 57.295779513f)
#  define DEG2RAD(d) ((d) * 0.01745329251994f)

#  define DISTANCE(xa, ya, xb, yb)                                          \
   sqrtf(((xb) - (xa)) * ((xb) - (xa)) + ((yb) - (ya)) * ((yb) - (ya)))

#  define SFDISTANCE(a, b)                                          \
   sqrtf(((b.x) - (a.x)) * ((b.x) - (a.x)) + ((b.y) - (a.y)) * ((b.y) - (a.y)))

#  define ARC_LENGTH(angle, radius) ((angle) * (radius))

#  define ROTATE(p, a) sf::Vector2f(cosf(a) * p.x - sinf(a) * p.y, \
                                    sinf(a) * p.x + cosf(a) * p.y)

#endif