//=====================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2022 Quentin Quadrat <lecrapouille@gmail.com>
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
// along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#include "City/City.hpp"

// g++ -Wall -Wextra -DZOOM=0.01f -I../../src/Simulator -I../../src _test.cpp -o prog -ldl /home/qq/MyGitHub/Drive/Drive/build/libcarsimulator.so `pkg-config --cflags --libs sfml-graphics`
// LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/qq/MyGitHub/Drive/Drive/build ./prog
int main()
{
// Test 1
    try
    {
        DynamicLoader loader("/lib64/libz.so.1", DynamicLoader::NOW);
        auto const zlibVersion = loader.prototype<char const* (void)>("zlibVersion");
        std::cout << "zlib version: " << zlibVersion() << std::endl;
    }
    catch(std::logic_error &e)
    {
        std::cerr << "Exception: "<<  e.what() << std::endl;
        return EXIT_FAILURE;
    }

// Test2
    DynamicLoader loader("/lib/x86_64-linux-gnu/libz.so.1.2.11", DynamicLoader::NOW);
    auto const zlibVersion = loader.prototype<char const* (void)>("zlibVersion");
    std::cout << "zlib version: " << zlibVersion() << std::endl;

// Test 3
    DynamicLoader loader("/home/qq/MyGitHub/Drive/Drive/simulations/sim_parking/build/simparking.so", DynamicLoader::LAZY);
    auto const simulation_name = loader.prototype<const char* (void)>("simulation_name");
    std::cout << "Simulation name: " << simulation_name() << std::endl;
    auto const create_city = loader.prototype<Car& (City&)>("create_city");
    City city;
    std::cout << city.parkings().size() << std::endl;
    create_city(city);
    std::cout << city.parkings().size() << std::endl;

    return EXIT_SUCCESS;
}
