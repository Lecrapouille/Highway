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

#ifndef DRAWABLE_HPP
#  define DRAWABLE_HPP

// *****************************************************************************
//! \file this file complete the SFML library by adding extra shapes to display.
// *****************************************************************************

#  include "Math/Units.hpp"
#  include <SFML/Graphics.hpp>

#  ifndef OUTLINE_THICKNESS
#    define OUTLINE_THICKNESS 0.01f
#  endif

// *****************************************************************************
//! \brief Show a grid used for debugging SpatialHashGrid.
// *****************************************************************************
class Grid final: public sf::Drawable
{
public:

    Grid(sf::Rect<float> const& bounds, sf::Vector2u const& dimensions, sf::Color color);

private:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        target.draw(m_lines.data(), m_lines.size(), sf::Lines, states);
    }

private:

    std::vector<sf::Vertex> m_lines;
};

// *****************************************************************************
//! \brief Non-filled circle used to draw turning radius.
// *****************************************************************************
class Circle final: public sf::Drawable
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor
    //! \param[in] x center position (X-axis) [meter].
    //! \param[in] y center position (Y-axis) [meter].
    //! \param[in] radius radius of the circle [meter].
    //! \param[in] color desired color (ie sf::Color::Red)
    //! \param[in] points (number of points composing the circle)
    //--------------------------------------------------------------------------
    Circle(Meter const x, Meter const y, Meter const radius, sf::Color const& color,
           std::size_t const points);

    //--------------------------------------------------------------------------
    //! \brief Default constructor
    //! \param[in] p center position (X-axis, Y-axis) [meter].
    //! \param[in] radius radius of the circle [meter].
    //! \param[in] color desired color (ie sf::Color::Red)
    //! \param[in] points (number of points composing the circle)
    //--------------------------------------------------------------------------
    Circle(sf::Vector2<Meter> const p, Meter const radius, sf::Color const& color,
           std::size_t const points)
        : Circle(p.x, p.y, radius, color, points)
    {}

private:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        target.draw(m_shape, states);
    }

private:

    sf::CircleShape m_shape;
};

// *****************************************************************************
//! \brief Arc used for drawing vehicle turning radius.
// *****************************************************************************
class Arc final: public sf::Drawable
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor
    //! \param[in] x center position (X-axis) [meter].
    //! \param[in] y center position (Y-axis) [meter].
    //! \param[in] radius radius of the circle [meter].
    //! \param[in] start initial angle [degree]
    //! \param[in] end final angle [degree]
    //! \param[in] color desired color (ie sf::Color::Red)
    //! \param[in] points (number of points composing the circle)
    //--------------------------------------------------------------------------
    Arc(Meter const x, Meter const y, Meter const radius, Degree const start,
        Degree const end, sf::Color const& color, std::size_t const points);

    //--------------------------------------------------------------------------
    //! \brief Dummy constructor. Shall be completed with init().
    //--------------------------------------------------------------------------
    Arc() = default;

    //--------------------------------------------------------------------------
    //! \brief Default constructor
    //! \param[in] x center position (X-axis) [meter].
    //! \param[in] y center position (Y-axis) [meter].
    //! \param[in] radius radius of the circle [meter].
    //! \param[in] start initial angle [degree]
    //! \param[in] end final angle [degree]
    //! \param[in] color desired color (ie sf::Color::Red)
    //! \param[in] points (number of points composing the circle)
    //--------------------------------------------------------------------------
    void init(Meter const x, Meter const y, Meter const radius, Degree const start,
              Degree const end, sf::Color const& color, std::size_t const points);

private:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        target.draw(m_shape, states);
    }

private:

    // *************************************************************************
    //! \brief Specialized shape representing an arc
    // *************************************************************************
    class ArcShape final: public sf::CircleShape
    {
    public:

        //----------------------------------------------------------------------
        //! \brief Default constructor.
        //! \param[in] radius: radius of the circle [meter]
        //! \param[in] starting angle [degree]
        //! \param[in] ending angle [degree]
        //! \param[in] points Number of points composing the arc.
        //----------------------------------------------------------------------
        ArcShape(Meter const radius, Degree const start, Degree const end,
                 std::size_t const points);

        //----------------------------------------------------------------------
        //! Dummy constructor.
        //----------------------------------------------------------------------
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
        virtual sf::Vector2f getPoint(std::size_t index) const override final;

    private:

        //! \brief Initial angle [rad].
        Radian m_start;
        //! \brief Final angle [rad].
        Radian m_end;
    };

    ArcShape m_shape;
};

// *****************************************************************************
//! \brief Draw an arrow used for debugging vehicle's turning radius.
// *****************************************************************************
class Arrow final: public sf::Drawable
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor.
    //! \param[in] a starting point.
    //! \param[in] b end point.
    //! \param[in] color desired color (ie sf::Color::Red)
    //--------------------------------------------------------------------------
    Arrow(sf::Vector2<Meter> const& a, sf::Vector2<Meter> const& b,
          sf::Color const& color)
        : Arrow(float(a.x.value()), float(a.y.value()),
                float(b.x.value()), float(b.y.value()), color)
    {}

    //--------------------------------------------------------------------------
    //! \brief Default constructor.
    //! \param[in] xa starting position along the X-axis [meter].
    //! \param[in] ya starting position along the Y-axis [meter].
    //! \param[in] xb ending position along the X-axis [meter].
    //! \param[in] yb ending position along the Y-axis [meter].
    //! \param[in] color desired color (ie sf::Color::Red)
    //--------------------------------------------------------------------------
    Arrow(Meter const xa, Meter const ya, Meter const xb, Meter const yb,
          sf::Color const& color)
        : Arrow(float(xa.value()), float(ya.value()),
                float(xb.value()), float(yb.value()), color)
    {}

private:

    //--------------------------------------------------------------------------
    //! \brief Default constructor.
    //! \param[in] xa starting position along the X-axis [meter].
    //! \param[in] ya starting position along the Y-axis [meter].
    //! \param[in] xb ending position along the X-axis [meter].
    //! \param[in] yb ending position along the Y-axis [meter].
    //! \param[in] color desired color (ie sf::Color::Red)
    //--------------------------------------------------------------------------
    Arrow(float const xa, float const ya, float const xb, float const yb,
          sf::Color const& color);

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        target.draw(m_tail, states);
        target.draw(m_head, states);
    }

private:

    sf::RectangleShape m_tail;
    sf::ConvexShape m_head;
};

#endif
