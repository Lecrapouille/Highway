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

#ifndef RK4_HPP
#  define RK4_HPP

#include <cmath>

//******************************************************************************
// https://github.com/Grubbly/4th-Order-Runge-Kutta-Differential-Equation-Approximation-Calculator/blob/master/RK4prob2.cpp
// https://gist.github.com/utk-dev/21a0f9f492bde5ded44a0425c0ba8df2
// https://github.com/Certaingemstone/SimpleRK4/blob/master/src/rk4.h
//******************************************************************************
template <typename Decimal>
class Integrator
{
    //--------------------------------------------------------------------------
    //! \brief Pointer function on differential equation (ie. q' = -t q)
    //! \param[in] t: the time value
    //! \param[in] q: unknown function of variable t
    //! return the computed value
    //--------------------------------------------------------------------------
    using Eqd = Decimal (*)(Decimal const t, Decimal const q);

public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor.
    //! \param[in] eqd: function on differential equation (ie. q' = -t q)
    //--------------------------------------------------------------------------
    Integrator(Eqd eqd)
        : f(eqd)
    {}

    //--------------------------------------------------------------------------
    //! \brief Implicit Euler method: q' = f(t, q(t))
    //! => q(t + dt) = q(t) + dt q'(t) = q(t) + dt f(t, q(t))
    //! \param[in] t: time value
    //! \param[inout] q: unknown function of variable t. This value is updated
    //! \param[in] dt: delta time
    //--------------------------------------------------------------------------
    void euler(Decimal const t, Decimal& q, Decimal const dt)
    {
        q = q + dt * f(t, q);
    }

    //--------------------------------------------------------------------------
    //! \brief Runge-Kutta method: q' = f(t, q(t))
    //! \param[in] t: time value
    //! \param[inout] q: unknown function of variable t. This value is updated
    //! \param[in] dt: delta time
    //--------------------------------------------------------------------------
    void rk4(Decimal const t, Decimal& q, Decimal const dt)
    {
        Decimal k1 = dt * f(t, q);
        Decimal k2 = dt * f(t + dt / Decimal(2.0), q + k1 / Decimal(2.0));
        Decimal k3 = dt * f(t + dt / Decimal(2.0), q + k2 / Decimal(2.0));
        Decimal k4 = dt * f(t + dt, q + k3);

        q = q + (k1 + Decimal(2.0) * k2 + Decimal(2.0) * k3 + k4) / Decimal(6.0);
    }

private:

    Eqd f;
};

#endif
