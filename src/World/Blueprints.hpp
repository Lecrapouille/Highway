// 2021 Quentin Quadrat quentin.quadrat@gmail.com
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

#  include "Vehicle/VehicleDimension.hpp"
#  include "World/ParkingDimension.hpp"
#  include <map>

// ****************************************************************************
//! \brief Database of car dimensions. You can interrogate it and complete it.
// ****************************************************************************
class CarDimensions
{
public:

    //--------------------------------------------------------------------------
    //! \brief Add a new car dimension in the database.
    //! \param[in] nane: non NULL string of the car "<maker>.<variant>"
    //! \param[in] dim the car dimension
    //--------------------------------------------------------------------------
    static bool add(const char* name, CarDimension const& dim)
    {
        assert(name != nullptr);
        return database().insert(std::map<std::string,
                                   CarDimension>::value_type(name, dim)).second;
    }

    //--------------------------------------------------------------------------
    //! \brief Get the car dimension from its name.
    //! \param[in] nane: non NULL string of the car "<maker>.<variant>"
    //! \return dim the car dimension (by copy).
    //! \return Throw exception if the car is unknown.
    //--------------------------------------------------------------------------
    static CarDimension get(const char* name)
    {
        assert(name != nullptr);
        return database().at(name);
    }

private:

    //--------------------------------------------------------------------------
    //! \brief Private static database.
    //--------------------------------------------------------------------------
    static std::map<std::string, CarDimension>& database()
    {
        static std::map<std::string, CarDimension> dico = {
            // https://www.renault-guyane.fr/cars/TWINGOB07Ph2h/DimensionsEtMotorisations.html
            { "Renault.Twingo", { 3.615f, 1.646f, 2.492f, 0.494f, 0.328f, 10.0f } },
            // https://www.largus.fr/images/images/ds3-crossback-dimensions-redimensionner.png
            { "Citroel.DS3", { 4.118f, 1.79f, 2.558f, 0.7f, 0.328f, 10.4f } },
            // https://www.vehikit.fr/nissan
            { "Nissan.NV200", { 4.321f, 1.219f, 2.725f, 0.840f, 0.241f, 10.6f} },
            // https://audimediacenter-a.akamaihd.net/system/production/media/78914/images/82a9fc874e33b8db4c849665c633c5148c3212d0/A196829_full.jpg?1582526293
            { "Audi.A6", { 4.951f, 1.902f, 2.924f, 1.105f, 0.328f, 11.7f } },
            { "Mini.Cooper", { 3.62f, 1.68f, 2.46f, 0.58f, 0.328f, 10.7f } },
        };
        return dico;
    }
};

// ****************************************************************************
//! \brief Class holding database of trailer dimensions. You can complete the
//! database.
// ****************************************************************************
class TrailerDimensions
{
public:

    //--------------------------------------------------------------------------
    //! \brief Add a new trailer dimension in the database.
    //! \param[in] nane: non NULL string of the car "<maker>.<variant>"
    //! \param[in] dim the car dimension
    //--------------------------------------------------------------------------
    static bool add(const char* name, TrailerDimension const& dim)
    {
        assert(name != nullptr);
        return database().insert(std::map<std::string,
                                 TrailerDimension>::value_type(name, dim)).second;
    }

    //--------------------------------------------------------------------------
    //! \brief Get the trailer dimension from its name.
    //! \param[in] nane: non NULL string of the car "<maker>.<variant>"
    //! \return dim the car dimension
    //! \return Throw exception if the trailer is unknown.
    //--------------------------------------------------------------------------
    static TrailerDimension get(const char* name)
    {
        assert(name != nullptr);
        return database().at(name);
    }

private:

    //--------------------------------------------------------------------------
    //! \brief Private static database.
    //--------------------------------------------------------------------------
    static std::map<std::string, TrailerDimension>& database()
    {
        static std::map<std::string, TrailerDimension> dico = {
            { "generic", { 1.646f, 1.646f, 2.5f, 0.494f, 0.2f } },
        };
        return dico;
    }
};


// ****************************************************************************
//! \brief Class holding database of parking dimensions. You can complete the
//! database.
// ****************************************************************************
class ParkingDimensions
{
public:

    //--------------------------------------------------------------------------
    //! \brief Add a new parking dimension in the database.
    //! \param[in] nane: non NULL string of the car "<type>.<angle>"
    //! \param[in] dim the parking dimension
    //--------------------------------------------------------------------------
    static bool add(const char* name, ParkingDimension const& dim)
    {
        assert(name != nullptr);
        return database().insert(std::map<std::string,
                                 ParkingDimension>::value_type(name, dim)).second;
    }

    //--------------------------------------------------------------------------
    //! \brief Get the parking dimension from its name.
    //! \param[in] nane: non NULL string of the car "<type>.<angle>"
    //! \return dim the parking dimension
    //! \return Throw exception if the trailer is unknown.
    //--------------------------------------------------------------------------
    static ParkingDimension get(const char* name)
    {
        assert(name != nullptr);
        return database().at(name);
    }

private:

    //--------------------------------------------------------------------------
    //! \brief Private static database.
    //--------------------------------------------------------------------------
    static std::map<std::string, ParkingDimension>& database()
    {
        // https://www.virages.com/Blog/Dimensions-Places-De-Parking
        static std::map<std::string, ParkingDimension> dico = {
            { "epi.0", { 5.0f, 2.0f, 0u } },
            { "epi.45", { 4.8f, 2.2f, 45u } },
            { "epi.60", { 5.15f, 2.25f, 60u } },
            { "epi.75", { 5.1f, 2.25f, 75u } },
            { "epi.90", { 5.0f, 2.3f, 90u } },
            { "creneau", { 5.0f, 2.0f, 0u } },
            { "bataille", { 5.0f, 2.3f, 90u } },
        };
        return dico;
    }
};

#endif
