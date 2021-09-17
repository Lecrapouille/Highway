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

#ifndef RENDERER_HPP
#  define RENDERER_HPP

#  include "Utils.hpp"
#  include <SFML/Graphics.hpp>

class Car;
class IACar;
class Trailer;
class Parking;

// *****************************************************************************
//! \brief Arc used to draw turning radius
// *****************************************************************************
class ArcShape: public sf::CircleShape
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor
    //! \param[in] radius: radius of the circle
    //! \param[in] starting angle [degree]
    //! \param[in] ending angle [degree]
    //! \param[in] Number of points composing the arc.
    //--------------------------------------------------------------------------
    ArcShape(float radius, float start, float end, std::size_t pointCount = 100u)
        : CircleShape(radius, pointCount + 2),
          m_start(DEG2RAD(start)),
          m_end(DEG2RAD(end))
    {}

    //--------------------------------------------------------------------------
    //! \brief Get a point of the circle.
    //! The returned point is in local coordinates, that is, the shape's
    //! transforms (position, rotation, scale) are not taken into account. The
    //! result is undefined if \a index is out of the valid range.
    //! \param index Index of the point to get, in range [0 .. getPointCount() - 1]
    //! \return index-th point of the shape.
    //--------------------------------------------------------------------------
    virtual sf::Vector2f getPoint(std::size_t index) const override
    {
        if ((index == 0) || (index == getPointCount()))
            return getOrigin();

        float const r = getRadius();
        float angle = m_start + float(index - 1) * m_end / float(getPointCount() -1);
        float x = std::cos(angle) * r;
        float y = std::sin(angle) * r;

        return sf::Vector2f(r + x, r + y);
    }

private:

    float m_start; // [deg]
    float m_end; // [deg]
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

    Arc(float x, float y, float r, float start, float end, sf::Color color = sf::Color::Red);

private:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        target.draw(m_shape, states);
    }

private:

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

    void draw(sf::RenderTarget& target, sf::RenderStates /*states*/) const override final
    {
        target.draw(m_tail);
        target.draw(m_head);
    }

private:

    sf::RectangleShape m_tail;
    sf::ConvexShape m_head;
};

void draw(Parking const& parking, sf::RenderTarget& target, sf::RenderStates const& states = sf::RenderStates::Default);
void draw(Car const& Car, sf::RenderTarget& target, sf::RenderStates const& states = sf::RenderStates::Default);
void draw(IACar const& Car, sf::RenderTarget& target, sf::RenderStates const& states = sf::RenderStates::Default);

#endif
