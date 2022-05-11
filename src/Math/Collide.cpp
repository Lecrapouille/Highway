// SAT collision check and resolution
// MIT license
// https://gist.github.com/eliasdaler/502b54fcf1b515bcc50360ce874e81bc

#include <array>
#include <cassert>
#include <cmath>
#include "Math/Collide.hpp"

using RectVertexArray = std::array<sf::Vector2f, 4>;
const float TOLERANCE = 0.0001f;

//------------------------------------------------------------------------------
// Returns normalized vector
static sf::Vector2f norm(const sf::Vector2f& v)
{
    const float length = sqrtf(v.x * v.x + v.y * v.y);
    if (length < TOLERANCE)
        return sf::Vector2f();
    return sf::Vector2f(v.x / length, v.y / length);
}

//------------------------------------------------------------------------------
static float dotProduct(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}

//------------------------------------------------------------------------------
// Returns right hand perpendicular vector
static sf::Vector2f getNormal(const sf::Vector2f& v)
{
    return sf::Vector2f(-v.y, v.x);
}

//------------------------------------------------------------------------------
// Find minimum and maximum projections of each vertex on the axis
static sf::Vector2f projectOnAxis(const RectVertexArray& vertices, const sf::Vector2f& axis)
{
    float min = std::numeric_limits<float>::infinity();
    float max = -std::numeric_limits<float>::infinity();
    for (auto& vertex : vertices)
    {
        float projection = dotProduct(vertex, axis);
        if (projection < min) { min = projection; }
        if (projection > max) { max = projection; }
    }
    return sf::Vector2f(min, max);
}

//------------------------------------------------------------------------------
// a and b are ranges and it's assumed that a.x <= a.y and b.x <= b.y
static bool areOverlapping(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x <= b.y && a.y >= b.x;
}

//------------------------------------------------------------------------------
// a and b are ranges and it's assumed that a.x <= a.y and b.x <= b.y
static float getOverlapLength(const sf::Vector2f& a, const sf::Vector2f& b)
{
    if (!areOverlapping(a, b)) { return 0.f; }
    return std::min(a.y, b.y) - std::max(a.x, b.x);
}

//------------------------------------------------------------------------------
static sf::Vector2f getCenter(const sf::RectangleShape& shape)
{
    const sf::Transform& transform = shape.getTransform();
    sf::FloatRect local = shape.getLocalBounds();
    return transform.transformPoint(local.width / 2.f, local.height / 2.f);
}

//------------------------------------------------------------------------------
static RectVertexArray getVertices(const sf::RectangleShape& shape)
{
    RectVertexArray vertices;
    const sf::Transform& transform = shape.getTransform();
    for (std::size_t i = 0u; i < shape.getPointCount(); ++i) {
        vertices[i] = transform.transformPoint(shape.getPoint(i));
    }
    return vertices;
}

//------------------------------------------------------------------------------
static sf::Vector2f getPerpendicularAxis(const RectVertexArray& vertices, std::size_t index)
{
    assert(index < 4);
    return getNormal(norm(vertices[index + 1] - vertices[index]));
}

//------------------------------------------------------------------------------
// Axes for which we'll test stuff. Two for each box, because testing for
// parallel axes isn't needed
static RectVertexArray
getPerpendicularAxes(const RectVertexArray& vertices1, const RectVertexArray& vertices2)
{
    RectVertexArray axes;

    axes[0] = getPerpendicularAxis(vertices1, 0);
    axes[1] = getPerpendicularAxis(vertices1, 1);

    axes[2] = getPerpendicularAxis(vertices2, 0);
    axes[3] = getPerpendicularAxis(vertices2, 1);
    return axes;
}

//------------------------------------------------------------------------------
// Separating Axis Theorem (SAT) collision test.
// Minimum Translation Vector (MTV) is returned for the first Oriented Bounding
// Box (OBB)
bool collide(const sf::RectangleShape& obb1, const sf::RectangleShape& obb2, sf::Vector2f& mtv)
{
    RectVertexArray vertices1 = getVertices(obb1);
    RectVertexArray vertices2 = getVertices(obb2);
    RectVertexArray axes = getPerpendicularAxes(vertices1, vertices2);

    // we need to find the minimal overlap and axis on which it happens
    float minOverlap = std::numeric_limits<float>::infinity();

    for (auto& axis : axes)
    {
        sf::Vector2f proj1 = projectOnAxis(vertices1, axis);
        sf::Vector2f proj2 = projectOnAxis(vertices2, axis);

        float overlap = getOverlapLength(proj1, proj2);
        if (overlap < TOLERANCE)
        {
            // shapes are not overlapping
            mtv = sf::Vector2f();
            return false;
        }
        else
        {
            if (overlap < minOverlap)
            {
                minOverlap = overlap;
                mtv = axis * minOverlap;
                // ideally we would do this only once for the minimal overlap
                // but this is very cheap operation
            }
        }
    }

    // Need to reverse MTV if center offset and overlap are not pointing in the
    // same direction
    bool notPointingInTheSameDirection = dotProduct(getCenter(obb1) - getCenter(obb2), mtv) < 0;
    if (notPointingInTheSameDirection)
    {
        mtv = -mtv;
    }
    return true;
}
