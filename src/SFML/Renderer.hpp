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

#ifndef RENDERER_HPP
#  define RENDERER_HPP

#  include "Utils.hpp"
#  include <SFML/Graphics.hpp>

#  define ZOOM 0.02f

class Car;
class Parking;

// *****************************************************************************
//! \brief Arc
// *****************************************************************************
class ArcShape: public sf::CircleShape
{
public:

    ArcShape(float radius, float start, float end, std::size_t pointCount)
        : CircleShape(radius, pointCount + 2),
          m_start(DEG2RAD(start)),
          m_end(DEG2RAD(end))
    {}

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

    float m_start;
    float m_end;
};

// *****************************************************************************
//! \brief Circle used to draw turning radii
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

// *****************************************************************************
//! \brief
// *****************************************************************************
class CarDrawable: public sf::Drawable
{
public:

    void bind(Car const& car);

protected:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;

private:

    Car const* m_car = nullptr;
    //! \brief The body drawn as a rectangle.
    mutable sf::RectangleShape m_body_shape;
    //! \brief The front wheel drawn as a rectangle.
    mutable sf::RectangleShape m_wheel_shape;
};

// *****************************************************************************
//! \brief
// *****************************************************************************
class ParkingDrawable: public sf::Drawable
{
public:

    void bind(Parking const& parking);

protected:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;

private:

    Parking const* m_parking = nullptr;
    //mutable sf::ConvexShape m_shape;
    mutable sf::RectangleShape m_shape;
};

#endif
