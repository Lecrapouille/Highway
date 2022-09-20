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

#ifndef MONITORING_HPP
#  define MONITORING_HPP

#  include <fstream>
#  include <map>
#  include <memory>

class Monitoring
{
public:

    //--------------------------------------------------------------------------
    ~Monitoring();

    //--------------------------------------------------------------------------
    template<typename T1, typename ... T2>
    void log(std::string const& name, const T1& head, const T2&... tail)
    {
        std::ofstream* fd = stream(name);
        if (fd != nullptr)
        {
            *fd << head << " ";
            log(*fd, tail...);
            *fd << ";" << std::endl;
        }
    }

    //--------------------------------------------------------------------------
    void close(std::string const& name)
    {
        std::ofstream* fd = stream(name);
        if (fd != nullptr)
        {
            *fd << "];" << std::endl;
        }
        m_files[name] = nullptr;
    }

private:

    //--------------------------------------------------------------------------
    template<typename T1, typename ... T2>
    void log(std::ofstream& fd, const T1& head, const T2&... tail)
    {
        fd << head << " ";
        log(fd, tail...);
    }

    //--------------------------------------------------------------------------
    void log(std::ofstream& fd)
    {
        // Do nothing
    }

    //--------------------------------------------------------------------------
    std::ofstream* stream(std::string const& name);

private:

    std::map<std::string, std::unique_ptr<std::ofstream>> m_files;
};

#endif
