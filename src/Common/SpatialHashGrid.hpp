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

#ifndef SPATIAL_HASH_GRID_HPP
#  define SPATIAL_HASH_GRID_HPP

#  include <SFML/Graphics/Rect.hpp>
#  include <vector>
#  include <cstdint>

//******************************************************************************
//! \brief
//! \note inspired by SimonDev's youtube video "Spatial Hash Grids & Tales from
//! Game Development" https://youtu.be/sx4IIQL0x7c
//******************************************************************************
class SpatialHashGrid
{
public:

    //**************************************************************************
    //! \brief
    //**************************************************************************
    struct Item
    {
        //! \brief Min and Max cell indices because the item can overlaps
        //! several cells.
        sf::Vector2u spatial_indices[2];
        //! \brief World position of the item.
        sf::Vector2f position;
        //! \brief bounding box of the item.
        sf::Vector2f dimension;
        //! \brief Unique id of the insertion query to be sure to insert only
        //! once an item since it can overlaps several cells.
        size_t query_id = 0u;
    };

    //--------------------------------------------------------------------------
    //! \brief Create a spatial hash grid and reserve memory.
    //! \param[in] bounds World coordinates of the grid.
    //! \param[in] dimension Number of grid cells (e.g. 3 by 2).
    //--------------------------------------------------------------------------
    SpatialHashGrid(sf::Rect<float> const& bounds, sf::Vector2u const& dimensions);

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void add(Item& item);

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void remove(Item& item);

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void update(Item& item);

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void clear();

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void findNear(Item& item, std::vector<Item*>& res);

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    sf::Rect<float> const& bounds() const
    {
        return m_bounds;
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    sf::Vector2u const& dimensions() const
    {
        return m_dimensions;
    }

private:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    inline uint32_t cell(uint32_t const x, uint32_t const y) const
    {
        return x + y * uint32_t(m_dimensions.x);
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    sf::Vector2u getIndices(sf::Vector2f p);

private:

    //! \brief Dimension of the grid covers
    sf::Rect<float> m_bounds;
    //! \brief Dimension of the grid cells (e.g. 3 by 2)
    sf::Vector2u m_dimensions;
    //! \brief
    std::vector<std::vector<Item*>> m_cells;
    //! \brief Is to be sure to insert only once an item since it can overlaps
    //! several cells.
    size_t m_query_ids = 1u;
};

#endif //SPATIAL_HASH_GRID_HPP
