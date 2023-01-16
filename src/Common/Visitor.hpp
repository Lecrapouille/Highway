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

#ifndef VISITOR_HPP
#  define VISITOR_HPP

#  include "MyLogger/Logger.hpp"

class Visitable;
class Radar;
class Antenna;

// ============================================================================
//! \brief Base class of visitor. visit() methods have been renamed as operator()
//! visit method are dummy used as fallbacks.
// ============================================================================
class Visitor
{
public:

    virtual ~Visitor() = default;

    virtual void operator()(Antenna&) {}
    virtual void operator()(Radar&) {}
    virtual void operator()(Visitable&)
    {
        LOGI("Dummy generic fallback action ...");
    }

    template<typename T>
    void fallback(T& object)
    {
        this->operator()(static_cast<Visitable&>(object));
    }
};

// ============================================================================
//! \brief Base class for accepting a visitor class.
// ============================================================================
class Visitable
{
public:

    virtual ~Visitable() = default;
    virtual void accept(Visitor&) = 0;
};

#endif
