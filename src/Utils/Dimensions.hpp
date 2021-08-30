// 2021 Quentin Quadrat lecrapouille@gmail.com
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

#ifndef DIMENSIONS_HPP
#  define DIMENSIONS_HPP

#  include "CarDimension.hpp"
#  include "ParkingDimension.hpp"
#  include <map>

class CarDimensions
{
public:

    static bool add(const char* n, CarDimension const& dim)
    {
        return dictionary().insert(std::map<std::string, CarDimension>::value_type(n, dim)).second;
    }

    static CarDimension get(const char* n)
    {
        return dictionary().at(n);
    }

private:

    static std::map<std::string, CarDimension>& dictionary()
    {
        static std::map<std::string, CarDimension> dico = {
            // https://www.renault-guyane.fr/cars/TWINGOB07Ph2h/DimensionsEtMotorisations.html
            { "Renault.Twingo", { 1.646f, 3.615f, 2.492f, 0.494f, 0.328f, 10.0f } },
            // https://www.largus.fr/images/images/ds3-crossback-dimensions-redimensionner.png
            { "Citroel.DS3", { 1.79f, 4.118f, 2.558f, 0.7f, 0.328f, 10.4f } },
            // https://www.vehikit.fr/nissan
            { "Nissan.NV200", { 1.219f, 4.321f, 2.725f, 0.840f, 0.241f, 10.6f} },
            // https://audimediacenter-a.akamaihd.net/system/production/media/78914/images/82a9fc874e33b8db4c849665c633c5148c3212d0/A196829_full.jpg?1582526293
            { "Audi.A6", { 1.886f, 4.93f, 2.924f, 1.095f, 0.328f, 11.7f } },
        };
        return dico;
    }
};

class TrailerDimensions
{
public:

    static bool add(const char* n, TrailerDimension const& dim)
    {
        return dictionary().insert(std::map<std::string, TrailerDimension>::value_type(n, dim)).second;
    }

    static TrailerDimension get(const char* n)
    {
        return dictionary().at(n);
    }

private:

    static std::map<std::string, TrailerDimension>& dictionary()
    {
        static std::map<std::string, TrailerDimension> dico = {
            { "generic", { 1.646f, 1.646f, 2.5f, 0.494f, 0.2f } },
        };
        return dico;
    }
};


class ParkingDimensions
{
public:

    static bool add(const char* n, ParkingDimension const& dim)
    {
        return dictionary().insert(std::map<std::string, ParkingDimension>::value_type(n, dim)).second;
    }

    static ParkingDimension get(const char* n)
    {
        return dictionary().at(n);
    }

private:

    static std::map<std::string, ParkingDimension>& dictionary()
    {
        // https://www.virages.com/Blog/Dimensions-Places-De-Parking
        static std::map<std::string, ParkingDimension> dico = {
            { "epi.0", { 5.0f, 2.0f, 0.0f } },
            { "epi.45", { 4.8f, 2.2f, 45.0f } },
            { "epi.60", { 5.15f, 2.25f, 60.0f } },
            { "epi.75", { 5.1f, 2.25f, 75.0f } },
            { "epi.90", { 5.0f, 2.3f, 90.0f } },
            { "creneau", { 5.0f, 2.0f, 0.0f } },
            { "bataille", { 5.0f, 2.3f, 90.0f } },
        };
        return dico;
    }
};

#endif
