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

#ifndef DRAWABLE_HPP
#  define DRAWABLE_HPP

// *****************************************************************************
//! \file this file complete the SFML library by adding extra shapes to display.
// *****************************************************************************

#  include "Math/Units.hpp"
#  include <SFML/Graphics.hpp>

// *****************************************************************************
//! \brief Show a grid used for debugging SpatialHashGrid.
// *****************************************************************************
class Grid: public sf::Drawable
{
public:

    Grid(sf::Rect<float> const& bounds, sf::Vector2u const& dimensions, sf::Color color);

private:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        target.draw(m_lines.data(), m_lines.size(), sf::Lines, states);
    }

private:

    std::vector<sf::Vertex> m_lines;
};

// *****************************************************************************
//! \brief Non-filled circle used to draw turning radius.
// *****************************************************************************
class Circle: public sf::Drawable
{
public:

    Circle(float x, float y, float r, sf::Color color = sf::Color::Red);
    Circle(Meter x, Meter y, float r, sf::Color color = sf::Color::Red)
      : Circle(float(x.value()), float(y.value()), r, color)
    {}

private:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        target.draw(m_shape, states);
    }

private:

    sf::CircleShape m_shape;
};

// *****************************************************************************
//! \brief Arc used for drawing vehicle turning radius.
// *****************************************************************************
class Arc: public sf::Drawable
{
public:

    //----------------------------------------------------------------------
    //! \brief Default constructor
    //! \param[in] x center position (X-axis).
    //! \param[in] y center position (Y-axis).
    //! \param[in] radius radius of the circle.
    //! \param[in] start angle [degree]
    //! \param[in] ending angle [degree]
    //! \param[in] color
    //----------------------------------------------------------------------
    Arc(float x, float y, float radius, Degree start, Degree end, sf::Color color);
    Arc(Meter x, Meter y, Meter radius, Degree start, Degree end, sf::Color color)
      : Arc(float(x.value()), float(y.value()), float(radius.value()), start, end, color)
    {}

    //----------------------------------------------------------------------
    //! \brief Dummy constructor. Shall be completed with init().
    //----------------------------------------------------------------------
    Arc() = default;

    //----------------------------------------------------------------------
    //! \brief Default constructor
    //! \param[in] x center position (X-axis).
    //! \param[in] y center position (Y-axis).
    //! \param[in] radius radius of the circle.
    //! \param[in] start angle [degree]
    //! \param[in] ending angle [degree]
    //! \param[in] color
    //----------------------------------------------------------------------
    void init(float x, float y, float r, Degree start, Degree end, sf::Color color);

private:

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override final
    {
        target.draw(m_shape, states);
    }

private:

    // *************************************************************************
    //! \brief
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
        ArcShape(float radius, Degree start, Degree end, std::size_t pointCount);
        ArcShape(Meter radius, Degree start, Degree end, std::size_t pointCount)
          :  ArcShape(float(radius.value()), start, end, pointCount)
        {}
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

        Degree m_start; // [deg]
        Degree m_end; // [deg]
    };

    ArcShape m_shape;
};

// *****************************************************************************
//! \brief Draw an arrow used for debugging vehicle's turning radius.
// *****************************************************************************
class Arrow: public sf::Drawable
{
public:

    Arrow(const float xa, const float ya, const float xb, const float yb, sf::Color color = sf::Color::Red);
    Arrow(const Meter xa, const Meter ya, const Meter xb, const Meter yb, sf::Color color = sf::Color::Red)
      : Arrow(float(xa.value()), float(ya.value()), float(xb.value()) ,float(yb.value()), color)
    {}
    
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
