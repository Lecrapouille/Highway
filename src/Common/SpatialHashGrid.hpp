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
