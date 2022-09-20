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

#include "Common/Monitoring.hpp"

Monitoring::~Monitoring()
{
    for (auto& it: m_files)
    {
        *it.second << "];" << std::endl;
    }
}

std::ofstream* Monitoring::stream(std::string const& name)
{
    auto it = m_files.find(name);
    if ((it == m_files.end()) || (it->second == nullptr))
    {
        auto ptr = std::make_unique<std::ofstream>();
        std::ofstream& fd = *ptr;

        fd.open(name + ".jl");
        if (fd.bad())
            return nullptr;

        //fd << "using Plots" << std::endl << std::endl;
        fd << "# Time [s], Throttle [%], Longitudinal vehicle speed [m/s], "
           << "Gravitational forces [N], Total longitudinal reistance load [N], "
           << "Aero forces [N], Load forces [N], Longitudinal vehicle "
           << "acceleration [m/s/s]" << std::endl;
        fd << "D=[" << std::endl;

        m_files[name] = std::move(ptr);
        return m_files[name].get();
    }
    return it->second.get();
}
