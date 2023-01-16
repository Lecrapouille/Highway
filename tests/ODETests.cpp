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

#include <iostream>
#include "Utils/RK4.hpp"

// First order equa diff
static float ed1(float const t, float const q)
{
    return -q * t;
}

int main()
{
    // Simulation settings
    float np = 30.0f; // number of points
    float tfin = 3.0f; // final time
    float dt = tfin / (np - 1.0); // step time

    float t = 0.0f; // time
    float qeuler = 1.0f; // Euler method: initial conditions
    float qrk4 = 1.0f; // RK4 method: initial conditions
    Integrator<float> integrator(ed1);

    // Generate code for Julia for ploting results
    std::cout << "using Plots" << std::endl << std::endl;
    std::cout << "# Time Euler RK4 Solution" << std::endl;
    std::cout << "A=[" << std::endl;
    for(int i = 1; i <= int(np); ++i)
    {
        std::cout << t
                  << " " << qeuler
                  << " " << qrk4
                  << " " << expf(-t*t / 2.0f) // real solution
                  << ";" << std::endl;

        // Two integration ways
        integrator.euler(t, qeuler, dt);
        integrator.rk4(t, qrk4, dt);
        t = t + dt;
    }
    std::cout << "];" << std::endl;
    std::cout << "plot(A[:,1], A[:,2], xlabel=\"time\", label=\"Euler\")" << std::endl;
    std::cout << "plot!(A[:,1], A[:,3], label=\"RK4\")" << std::endl;
    std::cout << "plot!(A[:,1], A[:,4], label=\"Solution\")" << std::endl;

    return 0;
}
