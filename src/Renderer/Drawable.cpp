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

#include "Renderer/Drawable.hpp"
#include "Math/Math.hpp"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cassert>

//------------------------------------------------------------------------------
Grid::Grid(sf::Rect<float> const& bounds, sf::Vector2u const& dimensions, sf::Color color)
{
    const float x = bounds.left;
    const float y = bounds.top;
    const float w = bounds.width;
    const float h = bounds.height;

    const float dx = bounds.width / float(dimensions.x);
    const float dy = bounds.height / float(dimensions.y);

    assert(dimensions.x != 0u);
    assert(dimensions.y != 0u);

    // Vertical lines
    for (uint32_t u = 0u; u <= dimensions.x; ++u)
    {
        m_lines.push_back(sf::Vertex(sf::Vector2f(x + dx * float(u), y), color));
        m_lines.push_back(sf::Vertex(sf::Vector2f(x + dx * float(u), y + h), color));
    }

    // Horizontal lines
    for (uint32_t u = 0u; u <= dimensions.y; ++u)
    {
        m_lines.push_back(sf::Vertex(sf::Vector2f(x, y + dy * float(u)), color));
        m_lines.push_back(sf::Vertex(sf::Vector2f(x + w, y + dy * float(u)), color));
    }
}

//------------------------------------------------------------------------------
Circle::Circle(Meter const x, Meter const y, Meter const radius, sf::Color const& color,
               std::size_t const points)
    : m_shape(float(radius.value()), points)
{
    m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());
    m_shape.setPosition(float(x.value()), float(y.value()));
    m_shape.setFillColor(sf::Color(255, 255, 255, 0));
    m_shape.setOutlineThickness(ZOOM * 2.0f);
    m_shape.setOutlineColor(color);
}

//------------------------------------------------------------------------------
Arc::Arc(Meter const x, Meter const y, Meter const radius, Degree const start,
         Degree const end, sf::Color const& color, std::size_t const points)
{
    init(x, y, radius, start, end, color, points);
}

//------------------------------------------------------------------------------
void Arc::init(Meter const x, Meter const y, Meter const radius, Degree const start,
               Degree const end, sf::Color const& color, std::size_t const points)
{
    m_shape = ArcShape(radius, start, end, points);

    m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());
    m_shape.setPosition(float(x.value()), float(y.value()));
    m_shape.setFillColor(sf::Color(255, 255, 255, 0));
    m_shape.setOutlineThickness(ZOOM * 2.0f);
    m_shape.setOutlineColor(color);
}

//------------------------------------------------------------------------------
Arrow::Arrow(const float xa, const float ya, const float xb, const float yb,
             sf::Color const& color)
{
    // Arc magnitude
    const float arrowLength = sqrtf((xb - xa) * (xb - xa) + (yb - ya) * (yb - ya));

    // Orientation
    const float teta = (yb - ya) / (xb - xa);
    float arrowAngle = std::atan(teta) * 180.0f / 3.1415f; // rad -> deg
    if (xb < xa)
        arrowAngle += 180.f;
    else if (yb < ya)
        arrowAngle += 360.f;

    // Head of the arrow
    const sf::Vector2f arrowHeadSize{ ZOOM * 9.0f, ZOOM * 9.0f };
    m_head = sf::ConvexShape{ 3 };
    m_head.setPoint(0, { 0.f, 0.f });
    m_head.setPoint(1, { arrowHeadSize.x, arrowHeadSize.y / 2.f });
    m_head.setPoint(2, { 0.f, arrowHeadSize.y });
    m_head.setOrigin(arrowHeadSize.x, arrowHeadSize.y / 2.f);
    m_head.setPosition(sf::Vector2f(xb, yb));
    m_head.setRotation(arrowAngle);
    m_head.setOutlineThickness(ZOOM);
    m_head.setOutlineColor(color);
    m_head.setFillColor(color);

    // Tail of the arrow.
    const sf::Vector2f tailSize{ arrowLength - arrowHeadSize.x, ZOOM * 1.0f };
    m_tail = sf::RectangleShape{ tailSize };
    m_tail.setOrigin(0.0f, tailSize.y / 2.f);
    m_tail.setPosition(sf::Vector2f(xa, ya));
    m_tail.setRotation(arrowAngle);
    m_tail.setOutlineThickness(ZOOM);
    m_tail.setOutlineColor(color);
    m_tail.setFillColor(color);
}

//------------------------------------------------------------------------------
Arc::ArcShape::ArcShape(Meter const radius, Degree const start, Degree const end,
                        std::size_t const points)
    : sf::CircleShape(float(radius.value()), points + 2u), m_start(start),
      m_end(end)
{}

//------------------------------------------------------------------------------
sf::Vector2f Arc::ArcShape::getPoint(std::size_t index) const
{
    if ((index == 0) || (index == getPointCount()))
        return getOrigin();

    float const r = getRadius();
    Degree angle = math::lerp_angle(m_start, m_end, float(index - 1u) / float(getPointCount() - 2u));
    float x = units::math::cos(angle) * r;
    float y = units::math::sin(angle) * r;

    return sf::Vector2f(r + x, r + y);
}
