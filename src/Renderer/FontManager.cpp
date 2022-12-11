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

#include "Renderer/FontManager.hpp"
#include <iostream>

// -----------------------------------------------------------------------------
bool FontManager::load(const char* name, const char* ttf)
{
    std::unique_ptr<sf::Font> font = std::make_unique<sf::Font>();
    if (font->loadFromFile(Path::instance().expand(ttf)))
    {
        m_fonts[name] = std::move(font);
        return true;
    }

    // An error is already displayed on the console
    return false;
}

// -----------------------------------------------------------------------------
sf::Font& FontManager::font(const char* name)
{
    try
    {
        return *m_fonts.at(name);
    }
    catch (...)
    {
        std::string e("Fatal: Unkown font '");
        throw std::runtime_error(e + name + "'");
    }
}
