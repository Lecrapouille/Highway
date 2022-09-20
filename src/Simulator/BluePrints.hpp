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

#ifndef BLUEPRINTS_HPP
#  define BLUEPRINTS_HPP

#  include <map>
#  include <unordered_map>
#  include <vector>
#  include <functional>
#  include <cassert>
#  include <cmath>
#  include <iostream>

// *****************************************************************************
//! \brief Database of blueprints
// *****************************************************************************
class BluePrints
{
public:

    //-------------------------------------------------------------------------
    //! \brief Init the database with a collection of predefined blueprints.
    //-------------------------------------------------------------------------
    static void init();

    //-------------------------------------------------------------------------
    //! \brief Init the database with a collection of predefined blueprints.
    //-------------------------------------------------------------------------
    template<class BLUEPRINT>
    static void init(std::map<std::string, BLUEPRINT> const& db)
    {
        database<BLUEPRINT>() = db;
    }

    //-------------------------------------------------------------------------
    //! \brief Add a new car blueprint in the database.
    //! \param[in] nane: non NULL string of the car "<maker>.<variant>"
    //! \param[in] dim the car blueprint
    //-------------------------------------------------------------------------
    template<class BLUEPRINT>
    static bool add(const char* name, BLUEPRINT const& blueprint)
    {
        assert(name != nullptr);
        if (!database<BLUEPRINT>().insert(
                typename std::map<std::string, BLUEPRINT>::value_type(name, blueprint)).second)
        {
            std::cerr << "Cannot add " << name << " in BluePrints database beause"
                      << " this entry already exists" << std::endl;
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
            return database<BLUEPRINT>().at(name);
        }
        catch (...)
        {
            std::string e("Fatal: Unkown blueprint '");
            throw std::runtime_error(e + name + "'");
        }
    }


private:

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    template<class BLUEPRINT>
    static std::map<std::string, BLUEPRINT>& database()
    {
        static std::unordered_map<std::string, std::map<std::string, BLUEPRINT>> m_databases;

        auto it = m_databases.find(typeid(BLUEPRINT).name());
        if (it == std::end(m_databases))
        {
            // Hold list of created heterogeneous stacks for their destruction
            m_erase_functions.emplace_back([](std::string& s)
            {
                std::cout << "erase database" << std::endl;
                m_databases.erase(s);
            });

            return m_databases[typeid(BLUEPRINT).name()];
        }
        return it->second;
    }

private:

    static std::vector<std::function<void(std::string&)>> m_erase_functions;
};

#endif
