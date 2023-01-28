//=============================================================================
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
//=============================================================================

#include "Simulator/Scenario.hpp"
#include "Common/DynamicLoader.hpp"

bool Scenario::loadSymbols(DynamicLoader& loader)
{
    if (loader)
    {
        name = loader.prototype<const char* (void)>("simulation_name");
        create = loader.prototype<Car& (Simulator&, City&)>("simulation_create_city");
        halt = loader.prototype<bool (Simulator const&)>("simulation_halt_when");
        react = loader.prototype<void(Simulator&, size_t)>("simulation_react_to");
        return true;
    }
    else
    {
        clear();
        return false;
    }
}