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
