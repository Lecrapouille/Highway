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

#  include <SFML/System/Clock.hpp>
#  include <SFML/Graphics.hpp>
#  include <Core/Math/Units.hpp>

// *****************************************************************************
//! \brief Display a message inside a colored rectangle. The message is faded
//! out after X seconds.
// *****************************************************************************
struct MessageBar : public sf::Drawable
{
    //--------------------------------------------------------------------------
    //! \brief Default constructor with a predefined fading duration [seconds].
    //! \note No message can be displayed until a font has been set with the \c
    // font() method.
    //--------------------------------------------------------------------------
    explicit MessageBar(Second const fading_duration = 2.5_s)
        : FADING_DURATION(fading_duration)
    {
        m_shape.setFillColor(sf::Color(100, 100, 100));
        m_shape.setOutlineThickness(-1);
        m_shape.setOutlineColor(sf::Color::Black);
        m_timer.restart();
    }

    //--------------------------------------------------------------------------
    //! \brief Change the font.
    //--------------------------------------------------------------------------
    MessageBar& font(sf::Font& font_)
    {
        m_text.setPosition(0, 0);
        m_text.setFont(font_);
        m_text.setCharacterSize(20);
        m_text.setFillColor(sf::Color::Black);
        return *this;
    }

    //--------------------------------------------------------------------------
    //! \brief Add a message to display. Replace the old one.
    //--------------------------------------------------------------------------
    MessageBar& entry(const std::string& message, sf::Color const& color)
    {
        if (message != m_message)
        {
            m_message = message;
            m_text.setString(m_message);
            m_count_lines = 1u + size_t(std::count(message.begin(), message.end(), '\n'));
            m_shape.setFillColor(color);
            reshape(float(m_shape.getSize().x));
        }
        m_timer.restart();
        return *this;
    }

    //--------------------------------------------------------------------------
    //! \brief Append the displayed messge. The color is not modified.
    //--------------------------------------------------------------------------
    MessageBar& append(std::string const& message)
    {
        if (m_text.getString().isEmpty())
        {
            m_text.setString(message);
        }
        else
        {
            m_text.setString(m_text.getString() + "\n" + message);
        }
        m_count_lines = 1u + size_t(std::count(message.begin(), message.end(), '\n'));
        reshape(float(m_shape.getSize().x));
        m_timer.restart();
        return *this;
    }

    //--------------------------------------------------------------------------
    //! \brief Return the lasted displayed message (even if faded away).
    //--------------------------------------------------------------------------
    std::string const& entry() const { return m_message; }

    //--------------------------------------------------------------------------
    //! \brief Update the size of the message box depending on its content.
    //--------------------------------------------------------------------------
    void reshape(float const width)
    {
        m_shape.setSize(sf::Vector2f(width,
                        float(m_text.getCharacterSize() + 4u) * float(m_count_lines)));
    }

    //--------------------------------------------------------------------------
    //! \brief Return if the message has faded out.
    //--------------------------------------------------------------------------
    inline bool faded() const
    {
        return double(m_timer.getElapsedTime().asSeconds()) >= FADING_DURATION.value();
    }

private: // Inherit from sf::Drawable

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual void draw(sf::RenderTarget& target, sf::RenderStates /*states*/) const override
    {
        if (!faded())
        {
            target.draw(m_shape);
            target.draw(m_text);
        }
    }

private:

    //! \brief Duration of the fading
    Second const FADING_DURATION;
    //! \brief Timer for removing the text
    sf::Clock m_timer;
    //! \brief Text displayed on the entry
    sf::Text m_text;
    //! \brief Handles appearance of the entry
    sf::RectangleShape m_shape;
    //! \brief String returned when the entry is activated
    std::string m_message;
    //! \brief For the width of the box.
    size_t m_count_lines = 0u;
};