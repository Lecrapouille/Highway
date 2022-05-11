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
