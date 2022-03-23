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
