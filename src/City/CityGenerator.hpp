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

#  include <queue>
#  include <vector>
#  include <list>
#  include <atomic>
#  include "Math/Math.hpp"
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
    class HeatMap
    {
    public:

        void generate(sf::Vector2<Meter> const& world_dimension, sf::Vector2u map_dimension);
        bool save(fs::path const& path);
        double get(sf::Vector2<Meter> const& p);

    private:

        sf::Vector2<Meter> m_world_dimension;
        sf::Vector2u m_map_dimension;
        sf::Vector2<double> m_scaling;
        sf::Image m_heatmap;
    };

    // *************************************************************************
    //! \brief Private representation of roads lighter representation than the
    //! \c Road class.
    // *************************************************************************
    class Road
    {
    public:

        //----------------------------------------------------------------------
        //! \brief Default Road constructor with given origin and destination,
        //! the time-step delay before this road is evaluated and if the road
        //! is a highway.
        //----------------------------------------------------------------------
        Road(sf::Vector2<Meter> const& from_, sf::Vector2<Meter> const& to_,
             size_t const priority_, bool const highway_)
            : id(m_next_id++), from(from_), to(to_), priority(priority_), highway(highway_)
        {
            std::cout << "       Constructor: " << *this << std::endl;
        }

        //----------------------------------------------------------------------
        //! \brief Copy constructor.
        //----------------------------------------------------------------------
        Road(Road const& other)
            : id(other.id), from(other.from), to(other.to), priority(other.priority),
                has_severed(other.has_severed), highway(other.highway), backwards(other.backwards),
                forwards(other.forwards), previous_segment_to_link(other.previous_segment_to_link)
        {
            std::cout << "       Copy Constructor: " << *this << std::endl;
        }

        Road(Road && other)
            : id(other.id), from(other.from), to(other.to), priority(other.priority),
              has_severed(other.has_severed), highway(other.highway), backwards(other.backwards),
              forwards(other.forwards), previous_segment_to_link(other.previous_segment_to_link)
        {
            std::cout << "       Move Constructor: " << *this << std::endl;
        }

        //----------------------------------------------------------------------
        //! \brief Copy operator.
        //----------------------------------------------------------------------
        Road& operator=(Road const& other)
        {
            this->~Road(); // destroy
            new (this) Road(other); // copy construct in place
            std::cout << "       Copy operator: " << *this << std::endl;
            return *this;
        }

        Road& operator=(Road && other)
        {
            this->~Road(); // destroy
            new (this) Road(other); // copy construct in place
            std::cout << "       Move operator: " << *this << std::endl;
            return *this;
        }

        //----------------------------------------------------------------------
        //! \brief
        //----------------------------------------------------------------------
        void setup_branch_links();

        //----------------------------------------------------------------------
        //! \brief
        //----------------------------------------------------------------------
	    std::vector<Road*>& links_for_end_containing(Road* road);

        //----------------------------------------------------------------------
        //! \brief
        //----------------------------------------------------------------------
        bool isInversed();

        //----------------------------------------------------------------------
        //! \brief Return the heading of the road [radian].
        //----------------------------------------------------------------------
        inline Radian heading() const { return math::orientation(from, to); }

        //----------------------------------------------------------------------
        //! \brief Return the magnitude of the road [meter].
        //----------------------------------------------------------------------
        inline Meter length() const { return math::distance(from, to); }

        friend std::ostream& operator<<(std::ostream& os, Road const& seg)
        {
            os << (seg.highway ? "Highway" : "Road") << seg.id
               << " ((" << seg.from.x << ", " << seg.from.y << ") => "
               << "(" << seg.to.x << ", " << seg.to.y << ")) "
               << " Prio: " << seg.priority << ", Sev: " << seg.has_severed;
            os << ", Backwards: [";
            for (auto const& it: seg.backwards)
                os << " " << it->id;
            os << " ], Forwards: [";
            for (auto const& it: seg.forwards)
                os << " " << it->id;
            os << " ]";
            return os;
        }

    private:

        //! \brief Static member saving the number of instances.
        static std::atomic<size_t> m_next_id;

    public:

        size_t const id;
        //! \brief World coordiante where the road starts (origin).
        sf::Vector2<Meter> from;
        //! \brief World coordiante where the road ends (destination).
        sf::Vector2<Meter> to;
        //! \brief Time-step delay before this road is evaluated.
        size_t priority;
        //! \brief Has the road splited another road ?
        bool has_severed = false;
        //! \brief Higway vs. basic roads.
        bool highway;
        //! \brief Backward links: are segments merged with this road segment at its origin point.
        std::vector<Road*> backwards;
        //! \brief Forward links are segments splited off at the destination point.
        std::vector<Road*> forwards;
        //! \brief Used for postponing the update for \c backwards and \c forwards.
        Road* previous_segment_to_link = nullptr;
    };

    // *************************************************************************
    //! \brief Settings for the generator.
    // *************************************************************************
    struct Config
    {
        //! \brief generate this number of roads: a higher limit produces
        //! larger networks.
        size_t max_roads = 2000u;
        //! \brief a road branching off at a 90 degree angle from an existing
        //! road can vary its direction by +/- this amount.
        Degree branch_angle_deviation = 3.0_deg;
        //! \brief a road continuing straight ahead from an existing road
        //! can vary its direction by +/- this amount.
        Degree straight_angle_deviation = 15.0_deg;
        //! \brief roads are allowed to intersect if they have a large enough
        //! difference in direction - this helps enforce grid-like networks
        Degree minimum_intersection_deviation = 30.0_deg;
        //! \brief try to produce 'normal' roads with this length if possible
        Meter default_road_length = 300.0_m;
        //! \brief try to produce 'highway' roads with this length if
        //! possible
        Meter highway_road_length = 400.0_m;
        //! \brief each 'normal' road has this probability of producing a
        //! branching road
        float default_branch_probability = 0.4f;
        //! \brief each 'highway' road has this probability of producing a
        //! branching road
        float highway_branch_probability = 0.05f;
        //! \brief only place 'normal' roads when the population is high
        //! enough
        double normal_branch_population_threshold = 128.0;
        //! \brief only place 'highway' roads when the population is high
        //! enough
        double highway_branch_population_threshold = 128.0;
        //! \brief delay branching from 'highways' by this amount to prevent
        //! them from being blocked by 'normal' roads
        size_t normal_branch_time_delay_from_highway = 5u;
        //! \brief allow a road to intersect with an existing road within
        //! this distance
        Meter max_snap_distance = 50.0_m;
        //! \brief select every nth road to place buildings around - a lower
        //! period produces denser building placement
        size_t building_road_period = 5u;
        //! \brief the number of buildings to generate per selected road
        size_t building_count_per_road = 10u;
        //! \brief the maximum distance that a building can be placed from a
        //! selected road
        Meter MAX_BUILDING_DISTANCE_FROM_SEGMENT = 400.0_m;
    };

    // *************************************************************************
    //! \brief Base class for creating roads in the same way of C++ functor.
    // *************************************************************************
    class GenerationRule
    {
    public:

        //-------------------------------------------------------------------------
        //! \brief Default constructor taking the reference to the city generator
        //! instance and the current time-step delay before the road is evaluated.
        //-------------------------------------------------------------------------
        GenerationRule(CityGenerator& context, size_t const priority_)
            : priority(priority_), m_context(context)
        {}

        //-------------------------------------------------------------------------
        //! \brief Needed because of pure virtual methods.
        //-------------------------------------------------------------------------
        virtual ~GenerationRule() = default;

        //-------------------------------------------------------------------------
        //! \brief Check if the rule can be apply.
        //-------------------------------------------------------------------------
        virtual bool accept(CityGenerator::Road& road, CityGenerator::Road& other) = 0;

        //-------------------------------------------------------------------------
        //! \brief Apply the rule for creating the road.
        //-------------------------------------------------------------------------
        virtual bool apply(CityGenerator::Road& road) = 0;

        //! \brief Rule priority.
        size_t const priority;

    protected:

        CityGenerator& m_context;
    };

    //-------------------------------------------------------------------------
    //! \brief Dummy constructor. Do nothing except storing generation rules.
    //-------------------------------------------------------------------------
    CityGenerator();

    //-------------------------------------------------------------------------
    //! \brief Generate city roads.
    //! \param[in] dimension city dimension [Meter x Meter].
    //-------------------------------------------------------------------------
    std::list<CityGenerator::Road*> const& generate(sf::Vector2<Meter> const& dimension);

    //-------------------------------------------------------------------------
    //! \brief Export the map of population density as PNG file.
    //-------------------------------------------------------------------------
    bool exportPopulationMap(fs::path const& path);

    //-------------------------------------------------------------------------
    //! FIXME shall not be public
    //! \brief The new road \c road will cross the road \c other at the given
    //! intersection \c intersection. This function update the junction states
    //! between the two roads.
    //! \param[in] road the new road crossing the older road.
    //! \param[in] other the older road.
    //! \param[in] intersection where roads are spliting together. This position
    //! shall be valid and will not be checked by this function.
    //-------------------------------------------------------------------------
    void junction(CityGenerator::Road& road, CityGenerator::Road& other,
                  sf::Vector2<Meter>& intersection);

private:

    //-------------------------------------------------------------------------
    //! \brief Generate the population density map.
    //-------------------------------------------------------------------------
    void generatePopulationMap();

    //-------------------------------------------------------------------------
    //! \brief Generate initial roads where other roads will follow.
    //-------------------------------------------------------------------------
    void generateInitialRoads(sf::Vector2<Meter> const& initial_position,
                              bool const highway);

    //-------------------------------------------------------------------------
    //! \brief Generate all the roads from initial roads.
    //! \return Return the reference to the list of created roads.
    //-------------------------------------------------------------------------
    std::list<CityGenerator::Road*> const& generateRoads();

    //-------------------------------------------------------------------------
    //! \brief Adjust the parameter values proposed by the \c globalGoals()
    //! function to the local environment.
    //-------------------------------------------------------------------------
    bool localConstraints(CityGenerator::Road& road);

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    void globalGoals(CityGenerator::Road& previous);

    //-------------------------------------------------------------------------
    //! \brief Used for highways or going straight on a normal branch.
    //! \return the new created road segment.
    //-------------------------------------------------------------------------
    Road continueRoad(Road const& previous, Radian const direction);

    //-------------------------------------------------------------------------
    //! \brief Used for branches extending from highways i.e. not highways
    //! themselves
    //! \return the new created road segment.
    //-------------------------------------------------------------------------
    Road branchRoad(Road const& previous, Radian const direction);

    //-------------------------------------------------------------------------
    //! \brief Return the population density to the given position.
    //-------------------------------------------------------------------------
    double population(sf::Vector2<Meter> const position);

    //-------------------------------------------------------------------------
    //! \brief Return the population density given the road. This function is
    //! used to follow the gradient of population density.
    //-------------------------------------------------------------------------
    double samplePopulation(Road const& road);

public:

    //! \brief Control the behavior of the city generator.
    CityGenerator::Config config;

private:

    // *************************************************************************
    //! \brief
    // *************************************************************************
    class Priority
    {
    public:
        bool operator() (Road const* road1, Road const* road2)
        {
            return road1->priority > road2->priority;
        }
    };

    //! \brief
    using PriorityQueue =
    std::priority_queue<Road*, std::vector<Road*>, Priority>;

    //! \brief Pending roads waiting for their operation.
    PriorityQueue m_pendings;
    //! \brief Map of population density.
    // TODO Idea: store 3 other maps (water, park, elevation, pedestrians)
    HeatMap m_population;
    //! \brief Generated roads (valid and invalid)
    std::list<CityGenerator::Road> m_branches;
    //! \brief Hold accepted roads.
    std::list<CityGenerator::Road*> m_roads;

    std::vector<std::unique_ptr<CityGenerator::GenerationRule>> m_rules;

    sf::Vector2<Meter> m_dimension;
};

#endif
