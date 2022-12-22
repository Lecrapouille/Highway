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

#include "Simulator/BluePrints.hpp"
#include "Simulator/City/City.hpp"

std::vector<std::function<void(std::string&)>> BluePrints::m_erase_functions;

//------------------------------------------------------------------------------
void BluePrints::init()
{
    //-------------------------------------------------------------------------
    //! \brief Database of car dimensions.
    //-------------------------------------------------------------------------
    BluePrints::init<CarBluePrint>(
        {
            // https://www.renault-guyane.fr/cars/TWINGOB07Ph2h/DimensionsEtMotorisations.html
            { "Renault.Twingo", { 3.615_m, 1.646_m, 2.492_m, 0.494_m, 0.328_m, 10.0_m } },
            // https://www.largus.fr/images/images/ds3-crossback-dimensions-redimensionner.png
            { "Citroen.DS3", { 4.118_m, 1.79_m, 2.558_m, 0.7_m, 0.328_m, 10.4_m } },
            // https://www.bsegfr.com/images/books/5/8/index.47.jpg
            { "Citroen.C3", { 3.941_m, 1.728_m, 2.466_m, 0.66_m, 0.328_m, 10.7_m } },
            // https://www.vehikit.fr/nissan
            { "Nissan.NV200", { 4.321_m, 1.219_m, 2.725_m, 0.840_m, 0.241_m, 10.6_m} },
            // https://audimediacenter-a.akamaihd.net/system/production/media/78914/images/82a9fc874e33b8db4c849665c633c5148c3212d0/A196829_full.jpg?1582526293
            { "Audi.A6", { 4.951_m, 1.902_m, 2.924_m, 1.105_m, 0.328_m, 11.7_m } },
            //
            { "Mini.Cooper", { 3.62_m, 1.68_m, 2.46_m, 0.58_m, 0.328_m, 10.7_m } },
        });

    //-------------------------------------------------------------------------
    //! \brief Database of trailer dimensions.
    //-------------------------------------------------------------------------
    BluePrints::init<TrailerBluePrint>(
        {
            { "generic", { 1.646_m, 1.646_m, 2.5_m, 0.494_m, 0.2_m } },
        });

    //-------------------------------------------------------------------------
    //! \brief Database of parking slot dimensions. For example:
    //! https://www.virages.com/Blog/Dimensions-Places-De-Parking
    //! ../../doc/pics/PerpendicularSpots.gif
    //! ../../doc/pics/DiagonalSpots.gif
    //! ../../doc/pics/ParallelSpots.jpg
    //-------------------------------------------------------------------------
    BluePrints::init<ParkingBluePrint>(
        {
            { "epi.0", { 5.0_m, 2.0_m, 0u } },
            { "epi.45", { 4.8_m, 2.2_m, 45u } },
            { "epi.60", { 5.15_m, 2.25_m, 60u } },
            { "epi.75", { 5.1_m, 2.25_m, 75u } },
            { "epi.90", { 5.0_m, 2.3_m, 90u } },
            { "creneau", { 5.0_m, 2.0_m, 0u } },
            { "bataille", { 5.0_m, 2.3_m, 90u } },
        });
}
