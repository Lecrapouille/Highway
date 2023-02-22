//=============================================================================
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
//=============================================================================

#ifndef CITY_GENERATOR_RULES_HPP
#  define CITY_GENERATOR_RULES_HPP

#  include "Math/Units.hpp"
#  include "City/CityGenerator.hpp"
#  include "MyLogger/Logger.hpp"
#  include <cassert>

// *****************************************************************************
//! \brief Dummy rule.
// *****************************************************************************
class DummyRule: public CityGenerator::GenerationRule
{
public:

    DummyRule(CityGenerator& context, size_t const priority_)
        : CityGenerator::GenerationRule(context, priority_)
    {}

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    virtual bool accept(CityGenerator::Road&, CityGenerator::Road&) override
    {
        return true;
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    virtual bool apply(CityGenerator::Road&) override
    {
        std::cout << "    YES (default)" << std::endl;
        return true;
    }
};

// *****************************************************************************
//! \brief Road intersecting road rule.
// *****************************************************************************
class IntersectingRoadsRule: public CityGenerator::GenerationRule
{
public:

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    IntersectingRoadsRule(CityGenerator& context, size_t const priority_)
        : CityGenerator::GenerationRule(context, priority_)
    {}

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    virtual bool accept(CityGenerator::Road& road, CityGenerator::Road& other) override
    {
        std::cout << "    Checking intersection:" << std::endl;
        // Roads do not intersect ?
        if (!math::intersect(std::make_tuple(road.from, road.to),
                             std::make_tuple(other.from, other.to),
                             m_intersection))
        {
            std::cout << "NO (do not intersect)" << std::endl;
            return false;
        }

        // Check the distance to the intersection.
        // (note: we are using [Meter^2] to avoid using sqrt())
        SquareMeter const d2 = math::distance2(road.from, m_intersection);
        if (d2 < m_previous_intersection_distance_squared)
        {
            // If intersecting lines are too similar don't accept
            // Degree const deviation = wrap_angle(other.direction() - road.direction());
            // if (deviation < m_context.config.minimum_intersection_deviation)
            // {
            //    std::cout << "        NO (previously)" << std::endl;
            //    return false;
            // }

            m_previous_intersection_distance_squared = d2;
            m_other = &other;
            std::cout << "        YES" << std::endl;
            return true;
        }

        std::cout << "        NO (previously)" << std::endl;
        return false;
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    virtual bool apply(CityGenerator::Road& road) override
    {
        std::cout << "IntersectingRoadsRule::apply" << std::endl;
        assert(m_other != nullptr);

        m_context.junction(road, *m_other, m_intersection);
        return true;
    }

private:

    sf::Vector2<Meter> m_intersection;
    CityGenerator::Road* m_other = nullptr;
    SquareMeter m_previous_intersection_distance_squared = 100000.0_m * 100000.0_m;
};

// *****************************************************************************
//! \brief Snap to crossing within radius rule.
// *****************************************************************************
class SnapToCrossingRule: public CityGenerator::GenerationRule
{
public:

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    SnapToCrossingRule(CityGenerator& context, size_t const priority_)
        : CityGenerator::GenerationRule(context, priority_)
    {}

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    virtual bool accept(CityGenerator::Road& road, CityGenerator::Road& other) override
    {
        std::cout << "    Checking radius crossing:" << std::endl;
        if (math::distance2(road.to, other.to) <=
            units::math::pow<2>(m_context.config.max_snap_distance))
        {
            m_other = &other;
            std::cout << "        YES" << std::endl;
            return true;
        }

        std::cout << "        NO" << std::endl;
        return false;
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    virtual bool apply(CityGenerator::Road& road) override
    {
        std::cout << "SnapToCrossingRule::apply" << std::endl;
        assert(m_other != nullptr);
        road.to = m_other->to;
        /*
        // Check for duplicate lines, don't add if it exists
        for (auto& link: links)
        {

        }

        if ((link.from.is_equal_approx(road.to) && link.end.is_equal_approx(road.from)) ||
        (link.from.is_equal_approx(road.from) && link.end.is_equal_approx(road.to)))
        {
        return false;
        }

        for (auto& link: links)
        {
        // Pick links of remaining roads at junction corresponding to other.end
        link.links_for_end_containing(m_other).append(road)
        // Add junction roads to snapped road
        road.links_f.append(link)
        }

        links.append(road)
        road.links_f.append(self.other)
        */
        return true;
    }

private:

    CityGenerator::Road* m_other = nullptr;
};

// *****************************************************************************
//! \brief Intersection within radius rule.
// *****************************************************************************
class RadiusIntersectionRule: public CityGenerator::GenerationRule
{
public:

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    RadiusIntersectionRule(CityGenerator& context, size_t const priority_)
        : CityGenerator::GenerationRule(context, priority_)
    {}

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    virtual bool accept(CityGenerator::Road& road, CityGenerator::Road& other) override
    {
        std::cout << "    Checking intersection:" << std::endl;

        math::Segment<Meter> seg(other.from, other.to);
        if (math::aligned(road.to, seg))
        {
            m_intersection = math::project(road.to, seg, false);
            SquareMeter d2 = math::dot(road.to, m_intersection);
            if (d2 < units::math::pow<2>(m_context.config.max_snap_distance))
            {
                // If intersecting lines are too similar don't accept
                // Degree const deviation = wrap_angle(other.direction() - road.direction());
                // if (deviation < m_context.config.minimum_intersection_deviation)
                // {
                //    std::cout << "        NO (alig)" << std::endl;
                //    return false;
                // }
                m_other = &other;
                std::cout << "        YES" << std::endl;
                return true;
            }
        }

        std::cout << "        NO (distance)" << std::endl;
        return false;
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    virtual bool apply(CityGenerator::Road& road) override
    {
        std::cout << "RadiusIntersectionRule::apply" << std::endl;
        assert(m_other != nullptr);

        m_context.junction(road, *m_other, m_intersection);
        return true;
    }

private:

    sf::Vector2<Meter> m_intersection;
    CityGenerator::Road* m_other = nullptr;
};

#endif