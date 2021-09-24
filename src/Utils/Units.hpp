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

#ifndef UNITS_HPP
#  define UNITS_HPP

// https://github.com/bernedom/SI
#include <SI/length.h>
#include <SI/angle.h>

using namespace SI::literals;

using Meter = SI::metre_t<float>;

template<class T>
using degree_t = SI::angle_t<T, std::ratio<10000, 572958>>;

using Degree = degree_t<float>;
using Radian = SI::angle_t<float, std::ratio<1>>;

constexpr degree_t<long double> operator""_deg(long double value) {
    return degree_t<long double>{value};
}

constexpr float cosf(Radian const a)
{
   return ::cosf(a.value());
}

constexpr float sinf(Radian const a)
{
   return ::sinf(a.value());
}

constexpr float cosf(Degree const d)
{
   return ::cosf(Radian(d));
}

constexpr float sinf(Degree const d)
{
   return ::sinf(Radian(d));
}

#endif // UNITS_HPP
