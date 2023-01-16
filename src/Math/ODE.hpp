//=====================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2023 Quentin Quadrat <lecrapouille@gmail.com>
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
// along with Highway.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================
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
