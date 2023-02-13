//==============================================================================
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
//==============================================================================

#ifndef CITY_GENERATOR_HPP
#  define CITY_GENERATOR_HPP

#  include <vector>
#  include "Math/Units.hpp"
#  include "Math/Perlin.hpp"
#  include "Common/FileSystem.hpp"

// *****************************************************************************
//! \brief
// *****************************************************************************
class CityGenerator
{
public:

    // *************************************************************************
    //! \brief
    // *************************************************************************
    class Segment
    {
    public:

        Segment(sf::Vector2<Meter> const& from_, sf::Vector2<Meter> const& to_,
                int const t_);

        Segment split(sf::Vector2<Meter> const& p);

        friend std::ostream& operator<<(std::ostream& os, Segment const& seg)
        {
            return os << "((" << seg.from.x << ", " << seg.from.y << "), "
                      << "(" << seg.to.x << ", " << seg.to.y << "))"
                      << std::endl;
        }

        bool highway = false;
        sf::Vector2<Meter> from;
        sf::Vector2<Meter> to;
        int t;
    };

    // *************************************************************************
    //! \brief
    // *************************************************************************
    struct Config
    {
        //! \brief generate this number of segments - a higher limit produces
        //! larger networks.
        size_t segment_count_limit = 2000u;
        //! \brief a segment branching off at a 90 degree angle from an existing
        //! segment can vary its direction by +/- this amount.
        Degree branch_angle_deviation = 3.0_deg;
        //! \brief a segment continuing straight ahead from an existing segment
        //! can vary its direction by +/- this amount.
        Degree straight_angle_deviation = 15.0_deg;
        //! \brief segments are allowed to intersect if they have a large enough
        //! difference in direction - this helps enforce grid-like networks
        Degree minimum_intersection_deviation = 30.0_deg;
        //! \brief try to produce 'normal' segments with this length if possible
        Meter default_segment_length = 300.0_m;
        //! \brief try to produce 'highway' segments with this length if
        //! possible
        Meter highway_segment_length = 400.0_m;
        //! \brief each 'normal' segment has this probability of producing a
        //! branching segment
        float default_branch_probability = 0.4f;
        //! \brief each 'highway' segment has this probability of producing a
        //! branching segment
        float highway_branch_probability = 0.05f;
        //! \brief only place 'normal' segments when the population is high
        //! enough
        float normal_branch_population_threshold = 0.5f;
        //! \brief only place 'highway' segments when the population is high
        //! enough
        float highway_branch_population_threshold = 0.5f;
        //! \brief delay branching from 'highways' by this amount to prevent
        //! them from being blocked by 'normal' segments
        float normal_branch_time_delay_from_highway = 5;
        //! \brief allow a segment to intersect with an existing segment within
        //! this distance
        Meter max_snap_distance = 50.0_m;
        //! \brief select every nth segment to place buildings around - a lower
        //! period produces denser building placement
        size_t building_segment_period = 5u;
        //! \brief the number of buildings to generate per selected segment
        size_t building_count_per_segment = 10u;
        //! \brief the maximum distance that a building can be placed from a
        //! selected segment
        Meter MAX_BUILDING_DISTANCE_FROM_SEGMENT = 400.0_m;
    };

    // *************************************************************************
    //! \brief Base class for creating roads.
    // *************************************************************************
    class GenerationRule
    {
    public:

        GenerationRule(CityGenerator& context, size_t const priority_)
            : priority(priority_), m_context(context)
        {}

        virtual ~GenerationRule() = default;

        //-------------------------------------------------------------------------
        //! \brief Check if the rule can be apply.
        //-------------------------------------------------------------------------
        virtual bool accept(CityGenerator::Segment& segment, CityGenerator::Segment& other) = 0;

        //-------------------------------------------------------------------------
        //! \brief Create road
        //-------------------------------------------------------------------------
        virtual bool apply(CityGenerator::Segment& segment) = 0;

        //! \brief Rule priority.
        size_t const priority;

    protected:

        CityGenerator& m_context;
    };

    //-------------------------------------------------------------------------
    //! \brief Dummy constructor. Do nothing except create generation rules.
    //-------------------------------------------------------------------------
    CityGenerator();

    //-------------------------------------------------------------------------
    //! \brief Generate city roads.
    //! \param[in] dimension city dimension [Meter x Meter].
    //-------------------------------------------------------------------------
    std::vector<Segment> const& create(sf::Vector2<Meter> const& dimension);

    //-------------------------------------------------------------------------
    //! \brief Export the map of population density to a PNG file.
    //-------------------------------------------------------------------------
    bool exportPopulationMap(fs::path const& path);

    //-------------------------------------------------------------------------
    //! \brief
    //! FIXME should be private
    //-------------------------------------------------------------------------
    void split(CityGenerator::Segment& segment, sf::Vector2<Meter>& p);

private:

    //-------------------------------------------------------------------------
    //! \brief Generate the map of population density.
    //-------------------------------------------------------------------------
    void generatePopulationMap();

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    void generateInitialSegments(sf::Vector2<Meter> const& initial_position);

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    std::vector<CityGenerator::Segment> const& generateSegments();

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    bool localConstraints(CityGenerator::Segment& segment);

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    std::vector<CityGenerator::Segment>& globalGoals(CityGenerator::Segment const& segment);

public:

    CityGenerator::Config config;

private:

    //! \brief
    enum class Rules {
        None, RadiusIntersection, SnapToCrossing, IntersectionCheck
    };

    //! \brief Dimension of the city [meter x meter].
    sf::Vector2<Meter> m_dimension;
    //! \brief Hold roads as segments.
    std::vector<Segment> m_segments;
    //! \brief Pending roads waiting for their operation.
    std::vector<Segment> m_pendings;
    //! \brief Map of population density.
    sf::Image m_heatmap;
    //! \brief List of rules for creating roads
    std::vector<std::unique_ptr<CityGenerator::GenerationRule>> m_rules;
};

// *****************************************************************************
//! \brief Dummy rule.
// *****************************************************************************
class DummyRule: public CityGenerator::GenerationRule
{
public:

    DummyRule(CityGenerator& context, size_t const priority_)
        : CityGenerator::GenerationRule(context, priority_)
    {}

    virtual bool accept(CityGenerator::Segment&, CityGenerator::Segment&) override
    {
        return true;
    }

    virtual bool apply(CityGenerator::Segment&) override
    {
        return true;
    }
};

// *****************************************************************************
//! \brief Road intersecting road rule.
// *****************************************************************************
class IntersectingRoadsRule: public CityGenerator::GenerationRule
{
public:

    IntersectingRoadsRule(CityGenerator& context, size_t const priority_)
        : CityGenerator::GenerationRule(context, priority_)
    {}

    virtual bool accept(CityGenerator::Segment& segment, CityGenerator::Segment& other) override;
    virtual bool apply(CityGenerator::Segment& segment) override;

private:

    sf::Vector2<Meter> m_intersection;
    CityGenerator::Segment* m_other = nullptr;
    SquareMeter m_previous_intersection_distance_squared = 100000.0_m * 100000.0_m;
};

// *****************************************************************************
//! \brief Snap to crossing within radius rule.
// *****************************************************************************
class SnapToCrossingRule: public CityGenerator::GenerationRule
{
public:

    SnapToCrossingRule(CityGenerator& context, size_t const priority_)
        : CityGenerator::GenerationRule(context, priority_)
    {}

    virtual bool accept(CityGenerator::Segment& segment, CityGenerator::Segment& other) override;
    virtual bool apply(CityGenerator::Segment& segment) override;

private:

    CityGenerator::Segment* m_other = nullptr;
};

// *****************************************************************************
//! \brief Intersection within radius rule.
// *****************************************************************************
class RadiusIntersectionRule: public CityGenerator::GenerationRule
{
public:

    RadiusIntersectionRule(CityGenerator& context, size_t const priority_)
        : CityGenerator::GenerationRule(context, priority_)
    {}

    virtual bool accept(CityGenerator::Segment& segment, CityGenerator::Segment& other) override;
    virtual bool apply(CityGenerator::Segment& segment) override;

private:

    sf::Vector2<Meter> m_intersection;
    CityGenerator::Segment* m_other = nullptr;
};

#endif
