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

#include "SpatialHashGrid.hpp"
#include <cassert>

//------------------------------------------------------------------------------
SpatialHashGrid::SpatialHashGrid(sf::Rect<float> const& bounds,
                                 sf::Vector2u const& dimensions)
    : m_bounds(bounds), m_dimensions(dimensions)
{
    assert(dimensions.x >= 1u);
    assert(dimensions.y >= 1u);
    m_cells.resize(dimensions.x * dimensions.y);
}

//------------------------------------------------------------------------------
void SpatialHashGrid::clear()
{
    m_query_ids = 1u;
    for (auto cell: m_cells)
    {
        cell.clear();
    }
    m_cells.clear();
}

//------------------------------------------------------------------------------
void SpatialHashGrid::add(Item& item)
{
    // query_id is to be sure to insert only once the item
    size_t query_id = m_query_ids++;

    const float x = item.position.x;
    const float y = item.position.y;
    const float w = item.dimension.x;
    const float h = item.dimension.y;

    item.spatial_indices[0] = getIndices({(x - w) / 2.0f, (y - h) / 2.0f});
    item.spatial_indices[1] = getIndices({(x + w) / 2.0f, (y + h) / 2.0f});

    const sf::Vector2u i0 = item.spatial_indices[0];
    const sf::Vector2u i1 = item.spatial_indices[1];
    for (uint32_t xn = i0.x; xn <= i1.x; ++xn)
    {
        for (uint32_t yn = i0.y; yn <= i1.y; ++yn)
        {
            if (item.query_id != query_id)
            {
                item.query_id = query_id;
                m_cells[cell(xn, yn)].push_back(&item);
            }
        }
    }
}

//------------------------------------------------------------------------------
sf::Vector2u SpatialHashGrid::getIndices(sf::Vector2f position)
{
    const float x = (position.x - m_bounds.left) / m_bounds.width;
    const float y = (position.y - m_bounds.top) / m_bounds.height;

    // We assume the position is not ouside the grid.
    assert((x >= 0.0f) && (x <= 1.0f));
    assert((y >= 0.0f) && (y <= 1.0f));

    const uint32_t xIndex = int(x * float(m_dimensions.x - 1u));
    const uint32_t yIndex = int(y * float(m_dimensions.y - 1u));

    return { xIndex, yIndex };
}

//------------------------------------------------------------------------------
void SpatialHashGrid::findNear(Item& item, std::vector<Item*>& res)
{
    const float x = item.position.x;
    const float y = item.position.y;
    const float w = item.dimension.x;
    const float h = item.dimension.y;

    const sf::Vector2u i0 = getIndices({(x - w) / 2.0f, (y - h) / 2.0f});
    const sf::Vector2u i1 = getIndices({(x + w) / 2.0f, (y + h) / 2.0f});

    res.clear();
    for (uint32_t xn = i0.x; xn <= i1.x; ++xn)
    {
        for (uint32_t yn = i0.y; yn <= i1.y; ++yn)
        {
            for (auto& it: m_cells[cell(xn, yn)])
            {
                res.push_back(it);
            }
        }
    }
}

//------------------------------------------------------------------------------
void SpatialHashGrid::update(Item& item)
{
    remove(item);
    add(item);
}

//------------------------------------------------------------------------------
void SpatialHashGrid::remove(Item& item)
{
    const sf::Vector2u i0 = item.spatial_indices[0];
    const sf::Vector2u i1 = item.spatial_indices[1];

    for (uint32_t xn = i0.x; xn <= i1.x; ++xn)
    {
        for (uint32_t yn = i0.y; yn <= i1.y; ++yn)
        {
            std::vector<Item*>& items = m_cells[cell(xn, yn)];

            // Search from the back
            size_t i = items.size();
            while (i--)
            {
                if (items[i] == &item)
                {
                    items[i] = items[items.size() - 1u];
                    items.pop_back();
                    return ;
                }
            }
        }
    }
}
