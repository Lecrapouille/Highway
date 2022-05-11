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

#ifndef FONT_MANAGER_HPP
#  define FONT_MANAGER_HPP

#  include "Common/Singleton.hpp"
#  include "Common/Path.hpp"
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
    //! \brief
    //--------------------------------------------------------------------------
    FontManager()
        : path(".:data:" DATADIR)
    {}

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

public:

    //! \brief Path to search ttf files
    Path path;

private:

    //! \brief List of fonts
    std::map<std::string, std::unique_ptr<sf::Font>> m_fonts;
};

#endif
