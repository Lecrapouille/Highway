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

#ifndef TRICYCLE_DYNAMIC_HPP
#  define TRICYCLE_DYNAMIC_HPP

#include "Vehicle/VehiclePhysics.hpp"

// ****************************************************************************
//! \brief
// ****************************************************************************
class PowerTrain
{
public:

    //-------------------------------------------------------------------------
    virtual ~PowerTrain() = default;

    //-------------------------------------------------------------------------
    float update(float const dt, float const throttle,
                 /*float const brake,*/ float const load);

private:

    //-------------------------------------------------------------------------
    // Throttle to engine torque using a simplified quadratic model.
    //-------------------------------------------------------------------------
    virtual float throttle2Torque(float const throttle, float const speed);

private:

    float m_torque_engine = 0.0f;
    float m_speed_engine = 100.0f;
    float m_gear_ratio = 0.35f;
    float m_inertia = 10.0f;

public:

    const float effective_radius = 0.3f;
};

// *****************************************************************************
//! \brief Simple car dynamic using the tricycle dynamic equations.
//! The position (x, y) of the car is the middle of the rear axle.
// *****************************************************************************
class TricycleDynamic: public VehiclePhysics
{
public:

    // https://github.com/quangnhat185/Self-driving_cars_toronto_coursera/blob/master/1.%20Introduciton%20to%20Self-driving%20Cars/Longitudinal_Vehicle_Model.ipynb
    virtual void update(CarControl const& control, float const dt) override;

private:

    PowerTrain m_powertrain;
    Monitoring m_monitor;
};

#endif
