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

#include "BluePrints.hpp"
#include "VehicleShape.hpp"
#include <iostream>

int main()
{
    BluePrints::init<CarBluePrint>(
        {
            // https://www.renault-guyane.fr/cars/TWINGOB07Ph2h/DimensionsEtMotorisations.html
            { "Renault.Twingo", { 3.615f, 1.646f, 2.492f, 0.494f, 0.328f, 10.0f } },
            // https://www.largus.fr/images/images/ds3-crossback-dimensions-redimensionner.png
            { "Citroen.DS3", { 4.118f, 1.79f, 2.558f, 0.7f, 0.328f, 10.4f } },
            // https://www.bsegfr.com/images/books/5/8/index.47.jpg
            { "Citroen.C3", { 3.941f, 1.728f, 2.466f, 0.66f, 0.328f, 10.7f } },
            // https://www.vehikit.fr/nissan
            { "Nissan.NV200", { 4.321f, 1.219f, 2.725f, 0.840f, 0.241f, 10.6f} },
            // https://audimediacenter-a.akamaihd.net/system/production/media/78914/images/82a9fc874e33b8db4c849665c633c5148c3212d0/A196829_full.jpg?1582526293
            { "Audi.A6", { 4.951f, 1.902f, 2.924f, 1.105f, 0.328f, 11.7f } },
            //
            { "QQ", { 3.9876841117376247f, 1.8508483613211535f, 2.6835034227027936f, 0.584285581986275f, 0.328f, 11.7f } },
            //
            { "Mini.Cooper", { 3.62f, 1.68f, 2.46f, 0.58f, 0.328f, 10.7f } },
        });

    BluePrints::init<TrailerBluePrint>(
        {
            { "generic", { 1.646f, 1.646f, 2.5f, 0.494f, 0.2f } },
        });

    BluePrints::add<CarBluePrint>("QQ", { 3.615f, 1.646f, 2.492f, 0.494f, 0.328f, 10.0f });

    CarBluePrint cbp = BluePrints::get<CarBluePrint>("Renault.Twingo");
    std::cout << cbp.length << std::endl;

    TrailerBluePrint tbp1 = BluePrints::get<TrailerBluePrint>("generic");
    std::cout << tbp1.length << std::endl;

    CarBluePrint tbp2 = BluePrints::get<CarBluePrint>("QQ");
    std::cout << tbp2.length << std::endl;


    auto d = BluePrints::get<CarBluePrint>("Renault.Twingo");
    VehicleShape<CarBluePrint> shape(d);

    return 0;
}
