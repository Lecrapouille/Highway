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

#pragma once

#  include <map>
#  include <memory>
#  include <functional>

// https://gist.github.com/facontidavide/c2d0fe6a29dc1e344e17799309ac2f1c

template<typename T>
class Factory
{
public:
    using RegistryMap = std::map<std::string, std::unique_ptr<T>>;

    std::unique_ptr<T> get(std::string const& name)
    {
        auto it = Factory::registry().find(name);
        return it == Factory::registry().end() ? nullptr : (it->second)();
    }

    void add(std::string const& name)
    {
        m_registry[name] = std::make_unique<T>();
    }

    inline RegistryMap& registry() const
    {
        return m_registry;
    }

private:
    RegistryMap m_registry;
};