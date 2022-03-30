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

#include "Vehicle/VehicleBluePrint.hpp"
#include "Math/Math.hpp"
#include <cassert>

constexpr float WHEEL_WIDTH = 0.1f;

//------------------------------------------------------------------------------
CarBluePrint::CarBluePrint(const float l, const float w, const float wb,
                           const float bo, const float wr, const float td)
    : length(l), width(w), track(width - WHEEL_WIDTH), wheelbase(wb),
      back_overhang(bo), front_overhang(length - wheelbase - back_overhang)
{
    max_steering_angle = asinf(wheelbase / (0.5f * td));
    assert(max_steering_angle > 0.0f);
    assert(max_steering_angle < DEG2RAD(90.0f));

    const float K = track / 2.0f;
    wheels[WheelName::FL].offset = sf::Vector2f(wheelbase, K);
    wheels[WheelName::FR].offset = sf::Vector2f(wheelbase, -K);
    wheels[WheelName::RL].offset = sf::Vector2f(0.0f, K);
    wheels[WheelName::RR].offset = sf::Vector2f(0.0f, -K);

    size_t i = WheelName::MAX;
    while (i--)
    {
        wheels[i].position = sf::Vector2f(NAN, NAN);
        wheels[i].radius = wr;
        wheels[i].width = WHEEL_WIDTH;
    }
}

//------------------------------------------------------------------------------
TrailerBluePrint::TrailerBluePrint(const float l, const float w, const float d,
                                   const float bo, const float wr)
    : length(l), width(w), track(width - WHEEL_WIDTH), wheelbase(d),
      back_overhang(bo)
{
    const float K = track / 2.0f;
    wheels[WheelName::RL].offset = sf::Vector2f(0.0f, -K);
    wheels[WheelName::RR].offset = sf::Vector2f(0.0f, K);

    size_t i = WheelName::MAX;
    while (i--)
    {
        wheels[i].position = sf::Vector2f(NAN, NAN);
        wheels[i].radius = wr;
        wheels[i].width = WHEEL_WIDTH;
    }
}
