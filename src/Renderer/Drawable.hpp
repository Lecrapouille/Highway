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

#ifndef DRAWABLE_HPP
#  define DRAWABLE_HPP

#  include <SFML/Graphics.hpp>

// *****************************************************************************
//! \brief
// *****************************************************************************
class Text: public sf::Drawable
{
public:

    Text();
    void position(float const x, float const y);
    Text& string(const char* str);
    Text& string(std::string const& str);
    Text& string(int const number);
    Text& string(float const number);

private:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        target.draw(m_text, states);
    }

    void update()
    {
        m_text.setPosition(m_x - m_text.getLocalBounds().width / 2.0f,
                           m_y - m_text.getLocalBounds().height);
    }

private:

    //! \brief SFML loaded font from a TTF file.
    sf::Font m_font;
    //! \brief SFML structure for rendering message.
    sf::Text m_text;
    float m_x = 0.0f;
    float m_y = 0.0f;
};

// *****************************************************************************
//! \brief Non-filled circle used to draw turning radius
// *****************************************************************************
class Circle: public sf::Drawable
{
public:

    Circle(float x, float y, float r, sf::Color color = sf::Color::Red);

private:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        target.draw(m_shape, states);
    }

private:

    sf::CircleShape m_shape;
};

// *****************************************************************************
//! \brief Circle used to draw turning radii
// *****************************************************************************
class Arc: public sf::Drawable
{
public:

    Arc(float x, float y, float r, float start, float end, sf::Color color);
    Arc() = default;
    void init(float x, float y, float r, float start, float end, sf::Color color);

private:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        target.draw(m_shape, states);
    }

private:

    // *************************************************************************
    //! \brief Arc used to draw turning radius
    // *************************************************************************
    class ArcShape: public sf::CircleShape
    {
    public:

        //----------------------------------------------------------------------
        //! \brief Default constructor
        //! \param[in] radius: radius of the circle
        //! \param[in] starting angle [degree]
        //! \param[in] ending angle [degree]
        //! \param[in] Number of points composing the arc.
        //----------------------------------------------------------------------
        ArcShape(float radius, float start, float end, std::size_t pointCount);
        ArcShape() = default;

        //----------------------------------------------------------------------
        //! \brief Get a point of the circle.
        //! The returned point is in local coordinates, that is, the shape's
        //! transforms (position, rotation, scale) are not taken into
        //! account. The result is undefined if \a index is out of the valid
        //! range.  \param index Index of the point to get, in range [0
        //! .. getPointCount() - 1]
        //! \return index-th point of the shape.
        //----------------------------------------------------------------------
        virtual sf::Vector2f getPoint(std::size_t index) const override;

    private:

        float m_start; // [deg]
        float m_end; // [deg]
    };

    ArcShape m_shape;
};

// *****************************************************************************
//! \brief Draw an arrow
// *****************************************************************************
class Arrow: public sf::Drawable
{
public:

    Arrow(const float xa, const float ya, const float xb, const float yb, sf::Color color = sf::Color::Red);

private:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        target.draw(m_tail, states);
        target.draw(m_head, states);
    }

private:

    sf::RectangleShape m_tail;
    sf::ConvexShape m_head;
};

#endif
