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

#include "Renderer/Drawable.hpp"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>

constexpr float RAD2DEG(float const r) { return r * 57.295779513f; }
constexpr float DEG2RAD(float const d) { return d * 0.01745329251994f; }

//------------------------------------------------------------------------------
Text::Text()
{
    if (!m_font.loadFromFile("data/font.ttf"))
    {
        std::cerr << "Failed loading fonts" << std::endl;
    }

    m_text.setFont(m_font);
    m_text.setCharacterSize(12.0f);
    m_text.setFillColor(sf::Color::Black);
}

//------------------------------------------------------------------------------
inline void Text::position(float const x, float const y)
{
    m_x = x;
    m_y = y;
}

//------------------------------------------------------------------------------
inline Text& Text::string(const char* str)
{
    m_text.setString(str);
    update();
    return *this;
}

//------------------------------------------------------------------------------
inline Text& Text::string(std::string const& str)
{
    m_text.setString(str);
    update();
    return *this;
}

//------------------------------------------------------------------------------
inline Text& Text::string(int const number)
{
    std::stringstream stream;
    stream << number;
    m_text.setString(stream.str());
    update();
    return *this;
}

//------------------------------------------------------------------------------
inline Text& Text::string(float const number)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << number;
    m_text.setString(stream.str());
    update();
    return *this;
}

//------------------------------------------------------------------------------
Circle::Circle(float x, float y, float r, sf::Color color)
    : m_shape(r, 100)
{
    m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());
    m_shape.setPosition(x, y);
    m_shape.setFillColor(sf::Color(255, 255, 255, 0));
    m_shape.setOutlineThickness(ZOOM * 2.0f);
    m_shape.setOutlineColor(color);
}

//------------------------------------------------------------------------------
Arc::Arc(float x, float y, float r, float start, float end, sf::Color color)
{
    init(x, y, r, start, end, color);
}

//------------------------------------------------------------------------------
void Arc::init(float x, float y, float r, float start, float end, sf::Color color)
{
    m_shape = ArcShape(r, start, end, 100);

    m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());
    m_shape.setPosition(x, y);
    m_shape.setFillColor(sf::Color(255, 255, 255, 0));
    m_shape.setOutlineThickness(ZOOM * 2.0f);
    m_shape.setOutlineColor(color);
}

//------------------------------------------------------------------------------
Arrow::Arrow(const float xa, const float ya, const float xb, const float yb, sf::Color color)
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
Arc::ArcShape::ArcShape(float radius, float start, float end, std::size_t pointCount)
    : CircleShape(radius, pointCount + 2),
      m_start(DEG2RAD(start)), m_end(DEG2RAD(end))
{}

//------------------------------------------------------------------------------
sf::Vector2f Arc::ArcShape::getPoint(std::size_t index) const
{
    if ((index == 0) || (index == getPointCount()))
        return getOrigin();

    float const r = getRadius();
    float angle = m_start + float(index - 1) * m_end / float(getPointCount() -1);
    float x = std::cos(angle) * r;
    float y = std::sin(angle) * r;

    return sf::Vector2f(r + x, r + y);
}
