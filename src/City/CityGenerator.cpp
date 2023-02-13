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

#include "project_info.hpp"
#include "Math/Math.hpp"
#include "City/CityGenerator.hpp"
#include "MyLogger/Logger.hpp"
#include <iostream>
#include <algorithm>
#include <tuple>
#include <cassert>

// -----------------------------------------------------------------------------
CityGenerator::Segment::Segment(sf::Vector2<Meter> const& from_,
                                sf::Vector2<Meter> const& to_, int const t_)
    : from(from_), to(to_), t(t_)
{}

// -----------------------------------------------------------------------------
CityGenerator::Segment CityGenerator::Segment::split(sf::Vector2<Meter> const& point)
{
    CityGenerator::Segment split_part(from, point, t);
    from = point;

    return split_part;
}

// -----------------------------------------------------------------------------
bool IntersectingRoadsRule::accept(CityGenerator::Segment& segment,
                                    CityGenerator::Segment& other)
{
    LOGD("IntersectingRoadsRule::accept");
    // Roads do not intersect ?
    if (!math::intersect(std::make_tuple(segment.from, segment.to),
                         std::make_tuple(other.from, other.to),
                         m_intersection))
    {
        LOGD("FALSE");
        return false;
    }

    // Check the distance to the intersection.
    // (note: we are using [Meter^2] to avoid using sqrt())
    SquareMeter const d2 = math::distance2(segment.from, m_intersection);
    if (d2 < m_previous_intersection_distance_squared)
    {
        // If intersecting lines are too similar don't accept
        //Degree const deviation = wrap_angle(other.direction() - segment.direction());
        //if (deviation < m_context.config.minimum_intersection_deviation)
        //    return false;

        m_previous_intersection_distance_squared = d2;
        m_other = &other;
        LOGD("TRUE");
        return true;
    }

LOGD("FALSE");
    return false;
}

// -----------------------------------------------------------------------------
bool IntersectingRoadsRule::apply(CityGenerator::Segment& segment)
{
LOGD("IntersectingRoadsRule::apply");
    assert(m_other != nullptr);
    m_context.split(*m_other, m_intersection);
	segment.to = m_intersection;
LOGD("TRUE");
    return true;
}

// -----------------------------------------------------------------------------
bool SnapToCrossingRule::accept(CityGenerator::Segment& segment, CityGenerator::Segment& other)
{
LOGD("SnapToCrossingRule::accept");
    if (math::distance2(segment.to, other.to) <=
        units::math::pow<2>(m_context.config.max_snap_distance))
    {
        m_other = &other;
        LOGD("TRUE");
        return true;
    }

LOGD("FALSE");
    return false;
}

// -----------------------------------------------------------------------------
bool SnapToCrossingRule::apply(CityGenerator::Segment& segment)
{
LOGD("SnapToCrossingRule::apply");
    assert(m_other != nullptr);
    segment.to = m_other->to;
/*
    // Check for duplicate lines, don't add if it exists
    for (auto& link: links)
    {

    }

    if ((link.from.is_equal_approx(segment.to) && link.end.is_equal_approx(segment.from)) ||
        (link.from.is_equal_approx(segment.from) && link.end.is_equal_approx(segment.to)))
    {
        return false;
    }

    for (auto& link: links)
    {
        // Pick links of remaining segments at junction corresponding to other.end
        link.links_for_end_containing(m_other).append(segment)
        // Add junction segments to snapped segment
        segment.links_f.append(link)
    }

    links.append(segment)
    segment.links_f.append(self.other)
*/
    return true;
}

// -----------------------------------------------------------------------------
bool RadiusIntersectionRule::accept(CityGenerator::Segment& segment, CityGenerator::Segment& other)
{
LOGD("RadiusIntersectionRule::accept");

    math::Segment<Meter> seg(other.from, other.to);
    if (math::aligned(segment.to, seg))
    {
        m_intersection = math::project(segment.to, seg, false);
        SquareMeter d2 = math::dot(segment.to, m_intersection);
        if (d2 < units::math::pow<2>(m_context.config.max_snap_distance))
        {
            // If intersecting lines are too similar don't accept
            //Degree const deviation = wrap_angle(other.direction() - segment.direction());
            //if (deviation < m_context.config.minimum_intersection_deviation)
            //    return false;

            m_other = &other;
            LOGD("TRUE");
            return true;
        }
    }

LOGD("FALSE");
    return false;
}

// -----------------------------------------------------------------------------
bool RadiusIntersectionRule::apply(CityGenerator::Segment& segment)
{
LOGD("RadiusIntersectionRule::apply");
    assert(m_other != nullptr);

    segment.to = m_intersection;
    m_context.split(*m_other, m_intersection);
    return true;
}

// -----------------------------------------------------------------------------
//    m_context.split(m_other, m_intersection);
//    m_other.split(m_intersection, segment, m_context.segments())
void CityGenerator::split(CityGenerator::Segment& segment, sf::Vector2<Meter>& point)
{
    m_segments.push_back(segment.split(point));
}

// -----------------------------------------------------------------------------
CityGenerator::CityGenerator()
{
    m_rules.emplace_back(std::make_unique<DummyRule>(*this, size_t(CityGenerator::Rules::None)));
    m_rules.emplace_back(std::make_unique<RadiusIntersectionRule>(*this, size_t(CityGenerator::Rules::RadiusIntersection)));
    m_rules.emplace_back(std::make_unique<SnapToCrossingRule>(*this, size_t(CityGenerator::Rules::SnapToCrossing)));
    m_rules.emplace_back(std::make_unique<IntersectingRoadsRule>(*this, size_t(CityGenerator::Rules::IntersectionCheck)));
}

// -----------------------------------------------------------------------------
std::vector<CityGenerator::Segment> const&
CityGenerator::create(sf::Vector2<Meter> const& dimension)
{
    m_dimension = dimension;
    generatePopulationMap();
    exportPopulationMap(project::info::tmp_path + "heatmap.png");
    m_segments.clear();
    m_pendings.clear();
    generateInitialSegments(sf::Vector2<Meter>(0.0_m, 0.0_m));
    generateSegments();

    return m_segments;
}

// -----------------------------------------------------------------------------
void CityGenerator::generatePopulationMap()
{
    sf::Vector2u dim(static_cast<unsigned int>(m_dimension.x),
                     static_cast<unsigned int>(m_dimension.y));
    perlin(m_heatmap, dim, [](double const x, double const y) -> sf::Color {
        double const dt = 1.0;
        auto const noise = (
            db::perlin(x / 64.0, y / 64.0, dt * 0.25) * 1.0 +
            db::perlin(x / 32.0, y / 32.0, dt * 0.75) * 0.5
        ) / 1.5;

        auto const brightness = sf::Uint8((noise * 0.5 + 0.5) * 255.0);
        return { brightness, brightness, brightness };
    });
}

// -----------------------------------------------------------------------------
bool CityGenerator::exportPopulationMap(fs::path const& path)
{
    sf::Texture texture;
    texture.loadFromImage(m_heatmap);
    return texture.copyToImage().saveToFile(path);
}

// -----------------------------------------------------------------------------
bool CityGenerator::localConstraints(CityGenerator::Segment& segment)
{
    CityGenerator::Rules priority = CityGenerator::Rules::None;
    CityGenerator::Rules action = CityGenerator::Rules::None;

    for (auto& other: m_segments)
    {
        for (auto& rule: m_rules)
        {
            if (size_t(priority) <= rule->priority)
            {
                if (rule->accept(segment, other))
                {
                    priority = CityGenerator::Rules(rule->priority);
                    action = priority;
                }
            }
        }
    }

    return m_rules[size_t(action)]->apply(segment);
}

// -----------------------------------------------------------------------------
void CityGenerator::generateInitialSegments(sf::Vector2<Meter> const& initial_position)
{
    m_pendings.push_back(Segment(initial_position, {config.highway_segment_length, 0.0_m}, 0));
    // TODO
    // m_pendings.push_back(Segment(initial_position, {-config.highway_segment_length, 0.0_m}, 0));
    // link initial roads together
}

// -----------------------------------------------------------------------------
std::vector<CityGenerator::Segment> const& CityGenerator::generateSegments()
{
    while ((m_pendings.size() >= 1u) && (m_segments.size() < config.segment_count_limit))
    {
        // Find the segment with the hightest priority (lower value)
        auto itseg = std::min_element(m_pendings.begin(), m_pendings.end(),
                                       [](Segment const& lhs, Segment const& rhs) { return lhs.t > rhs.t; });
        Segment seg = *itseg;
        m_pendings.erase(itseg);

std::cout << "Seg " << seg << std::endl;
        if (localConstraints(seg))
        {
            std::cout << "OUI" << std::endl;
            m_segments.push_back(seg);
            for (auto& it: globalGoals(seg))
            { // TODO deplacer la boucle dans globalGoals()
                it.t += seg.t + 1u;
                m_pendings.push_back(it);
            }
        }
    }

    return m_segments;
}

// -----------------------------------------------------------------------------
std::vector<CityGenerator::Segment>&
CityGenerator::globalGoals(CityGenerator::Segment const& segment)
{
    static std::vector<CityGenerator::Segment> dummy; // TODO
    return dummy;
}

/*
int main()
{
    CityGenerator g;
    g.create(sf::Vector2u(800u, 600u));

    std::vector<Segment> const& segments = g.generateSegments(sf::sf::Vector2f(0.0f, 0.0f), sf::sf::Vector2f(100.0f, 0.0f), 10u);
    for (auto const& it: segments)
    {
        std::cout << "Segment (" << it.from.x << ", " << it.from.y << "), ("
                  << it.to.x << ", " << it.to.y << ")" << std::endl;
    }

    return 0;
}
*/
