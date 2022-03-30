//=====================================================================
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
//=====================================================================
// TimedPetriNetEditor: A timed Petri net editor.
// Copyright 2021 Quentin Quadrat <lecrapouille@gmail.com>
//
// This file is part of TimedPetriNetEditor.
//
// TimedPetriNetEditor is free software: you can redistribute it and/or modify it
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

#ifndef MESSAGEBAR_HPP
#  define MESSAGEBAR_HPP

#  include <iostream>

// *****************************************************************************
//! \brief A text inside a rectangle
// *****************************************************************************
struct MessageBar : public sf::Drawable
{
    MessageBar()
    {
        m_text.setPosition(0, 0);
        m_text.setCharacterSize(20);
        m_text.setFillColor(sf::Color::Black);

        m_shape.setFillColor(sf::Color(100,100,100));
        m_shape.setOutlineThickness(-1);
        m_shape.setOutlineColor(sf::Color::Black);

        m_timer.restart();
    }

    void setFont(sf::Font& font)
    {
        m_text.setFont(font);
    }

    void setText(const std::string& message)
    {
        std::cout << message << std::endl;
        m_message = message;
        m_text.setString(m_message);
        m_timer.restart();
    }

    void setSize(sf::Vector2u const& dimensions)
    {
        m_shape.setSize(sf::Vector2f(float(dimensions.x), 25.0f));
    }

private:

    void draw(sf::RenderTarget& target, sf::RenderStates /*states*/) const override final
    {
        const float BLINK_PERIOD = 2.5f;

        float period = m_timer.getElapsedTime().asSeconds();
        if (period >= BLINK_PERIOD)
        {
            period = BLINK_PERIOD;
        }
        else
        {
            target.draw(m_shape);
            target.draw(m_text);
        }
    }

private:

    //! \brief Timer for removing the text
    sf::Clock m_timer;

    //! \brief Text displayed on the entry
    sf::Text m_text;

    //! \brief Handles appearance of the entry
    sf::RectangleShape m_shape;

    //! \brief String returned when the entry is activated
    std::string m_message;
};

#endif
