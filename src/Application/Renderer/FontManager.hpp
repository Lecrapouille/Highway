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

#  include "Core/Common/Singleton.hpp"
#  include <SFML/Graphics.hpp>
#  include <memory>
#  include <map>

//******************************************************************************
//! \brief Manage TTF fonts has std::map
//******************************************************************************
class FontManager: public Singleton<FontManager>
{
public:

    //--------------------------------------------------------------------------
    //! \brief Store a new font.
    //! \param[in] name: the internal name of the font.
    //! \param[in] ttf: the filename of the font. It will be searched in the
    //! following oreder: ttf, ./data/., DATADIR
    //! \return true if the font has been loaded with success.
    //--------------------------------------------------------------------------
    bool load(const char* ttf, const char* name);

    //--------------------------------------------------------------------------
    //! \brief Get the font given its internal name.
    //! \return the reference to the font if present.
    //! \throw if the font was not found.
    //--------------------------------------------------------------------------
    sf::Font& font(const char* name);

private:

    //! \brief List of fonts
    std::map<std::string, std::unique_ptr<sf::Font>> m_fonts;
};