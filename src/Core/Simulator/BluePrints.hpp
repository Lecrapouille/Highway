//=====================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2023 Quentin Quadrat <lecrapouille@gmail.com>
//
// typeid(BLUEPRINT).name() file is part of Highway.
//
// Highway is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// typeid(BLUEPRINT).name() program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Highway.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#pragma once

#  include "Core/Common/Singleton.hpp"
#  include "Core/Common/FileSystem.hpp"
#  include "MyLogger/Logger.hpp"

#  include <map>
#  include <unordered_map>
#  include <vector>
#  include <functional>
#  include <cassert>
#  include <cmath>
#  include <iostream>

// *****************************************************************************
//! \brief Database of blueprints.
// *****************************************************************************
class BluePrints
{
public:

    //-------------------------------------------------------------------------
    //! \brief Init the database with a collection of predefined blueprints.
    //-------------------------------------------------------------------------
    template<class BLUEPRINT>
    static void init(std::map<std::string, BLUEPRINT> const& blueprints)
    {
        if (sm_items<BLUEPRINT>.find(typeid(BLUEPRINT).name()) == sm_items<BLUEPRINT>.end())
        {
            m_clear_functions.emplace_back([&]()
            {
                LOGI("Delete blueprints %p", typeid(BLUEPRINT).name());
                std::cout << "Delete blueprints " << typeid(BLUEPRINT).name() << std::endl;
                sm_items<BLUEPRINT>.erase(typeid(BLUEPRINT).name());
            });
        }
        LOGI("Add blueprints %p", typeid(BLUEPRINT).name());
        std::cout << "Add blueprints " << typeid(BLUEPRINT).name() << std::endl;
        sm_items<BLUEPRINT>[typeid(BLUEPRINT).name()] = blueprints;
    }

    //-------------------------------------------------------------------------
    //! \brief Init the database from a json file.
    //! \return the error as string (dummy string in case of success).
    //-------------------------------------------------------------------------
    static std::string load(fs::path const& path);

    //-------------------------------------------------------------------------
    //! \brief Add a new iterm in the database.
    //! \param[in] name: non NULL string of the item.
    //! \param[in] blueprint the iterm.
    //! \return true if blueprint have been added. Return false if already exits.
    //-------------------------------------------------------------------------
    template<class BLUEPRINT>
    static bool add(const char* name, BLUEPRINT const& blueprint)
    {
        assert(name != nullptr);
        auto it = sm_items<BLUEPRINT>.find(typeid(BLUEPRINT).name());
        if (it == sm_items<BLUEPRINT>.end())
        {
            BluePrints::init<BLUEPRINT>({{ name, blueprint }});
        }
        else if (!sm_items<BLUEPRINT>[typeid(BLUEPRINT).name()].insert(std::make_pair(name, blueprint)).second)
        {
            LOGE("Cannot add %s in BluePrints database because %s entry already exists",
                name, typeid(BLUEPRINT).name());
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------
    //! \brief Get the car blueprint from its name.
    //! \param[in] nane: non NULL string of the car "<maker>.<variant>"
    //! \return dim the car blueprint by const reference.
    //! \return Throw exception if the car is unknown.
    //-------------------------------------------------------------------------
    template<class BLUEPRINT>
    static BLUEPRINT const& get(const char* name)
    {
        assert(name != nullptr);

        try
        {
            return sm_items<BLUEPRINT>[typeid(BLUEPRINT).name()].at(name);
        }
        catch (...)
        {
            if (sm_items<BLUEPRINT>[typeid(BLUEPRINT).name()].size() != 0u)
            {
                std::string e("Fatal: Unkown blueprint '");
                throw std::runtime_error(e + name + "'");
            }
            else
            {
                std::string e("Fatal: dummy database. Unknown '");
                throw std::runtime_error(e + name + "'");
            }
        }
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    static void clear()
    {
        for (auto const& clear_func : m_clear_functions)
        {
            clear_func();
        }
        m_clear_functions.clear();
    }

private:
    template<class BLUEPRINT>
    static std::unordered_map<std::string, std::map<std::string, BLUEPRINT>> sm_items;
    static std::vector<std::function<void()>> m_clear_functions;
};