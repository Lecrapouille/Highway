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

#include "City/CityGenerator.hpp"
#include "City/CityGeneratorRules.hpp"
#include "project_info.hpp"
#include "Math/Random.hpp"
#include <iostream>
#include <algorithm>

std::atomic<size_t> CityGenerator::Road::m_next_id(1U);

// -----------------------------------------------------------------------------
static double non_linear_distribution(double const limit)
{
    // Non-linear distribution
    double const non_uniform_norm = limit * limit * limit;
    double val;
    do
    {
        val = Random::get(-limit, limit);
    } while (Random::get(0.0, 1.0) < (val * val * val / non_uniform_norm));

    return val;
}

// -----------------------------------------------------------------------------
static Radian random_angle(Radian const limit)
{
    return Radian(non_linear_distribution(limit.value()));
}

// -----------------------------------------------------------------------------
void CityGenerator::HeatMap::generate(sf::Vector2<Meter> const &world_dimension, sf::Vector2u map_dimension)
{
    m_world_dimension = world_dimension;
    m_map_dimension = map_dimension;
    m_scaling.x = world_dimension.x.value() / double(map_dimension.x);
    m_scaling.y = world_dimension.y.value() / double(map_dimension.y);

    perlin(m_heatmap, m_map_dimension, [](double const x, double const y) -> sf::Color
           {
        double const dt = 1.0;
        auto const noise = (
            db::perlin(x / 64.0, y / 64.0, dt * 0.25) * 1.0 +
            db::perlin(x / 32.0, y / 32.0, dt * 0.75) * 0.5
        ) / 1.5;

        auto const brightness = sf::Uint8((noise * 0.5 + 0.5) * 255.0);
        return { brightness, brightness, brightness, 255 }; });
}

// -----------------------------------------------------------------------------
bool CityGenerator::HeatMap::save(fs::path const &path)
{
    sf::Texture texture;
    texture.loadFromImage(m_heatmap);
    return texture.copyToImage().saveToFile(path);
}

// -----------------------------------------------------------------------------
double CityGenerator::HeatMap::get(sf::Vector2<Meter> const &p)
{
    const Meter x = math::map(p.x, -m_world_dimension.x / 2.0, m_world_dimension.x / 2.0, 0.0_m, Meter(m_map_dimension.x));
    const Meter y = math::map(p.y, -m_world_dimension.y / 2.0, m_world_dimension.y / 2.0, 0.0_m, Meter(m_map_dimension.y));
    const unsigned int u = static_cast<unsigned int>(x.value());
    const unsigned int v = static_cast<unsigned int>(y.value());

    // std::cout << "Heatmap(" << p.x << ", " << p.y << ") => (" << u << ", " << v << "): "
    //<< int(m_heatmap.getPixel(u, v).r) << std::endl;

    return double(m_heatmap.getPixel(u, v).r);
}

// -----------------------------------------------------------------------------
// FIXME: how to remove this: this is due to the fact initial roads start with opposite direction
bool CityGenerator::Road::isInversed()
{
    const SquareMeter epsilon = 0.1_m * 1.0_m;

    if (backwards.size() > 0u)
    {
        return (math::distance2(backwards[0]->from, from) < epsilon) ||
               (math::distance2(backwards[0]->to, from) < epsilon);
    }
    else if (forwards.size() > 0u)
    {
        return (math::distance2(forwards[0]->from, to) < epsilon) ||
               (math::distance2(forwards[0]->to, to) < epsilon);
    }

    return false;
}

// -----------------------------------------------------------------------------
void CityGenerator::Road::setup_branch_links()
{
    if (previous_segment_to_link == nullptr)
        return;

    for (auto &link : previous_segment_to_link->forwards)
    {
        backwards.push_back(link);
        if (std::find(backwards.begin(), backwards.end(), previous_segment_to_link) != backwards.end())
        {
            backwards.push_back(this);
        }
        else if (std::find(forwards.begin(), forwards.end(), previous_segment_to_link) != forwards.end())
        {
            forwards.push_back(this);
        }
    }
    previous_segment_to_link->forwards.push_back(this);
    backwards.push_back(previous_segment_to_link);
}

// -----------------------------------------------------------------------------
std::vector<CityGenerator::Road *> &CityGenerator::Road::links_for_end_containing(Road *road)
{
    if (std::find(backwards.begin(), backwards.end(), road) != backwards.end())
        return backwards;
    auto it = std::find(forwards.begin(), forwards.end(), road);
    assert(it != forwards.end());
    return forwards;
}

// -----------------------------------------------------------------------------
void CityGenerator::junction(CityGenerator::Road &road, CityGenerator::Road &other,
                             sf::Vector2<Meter> &intersection)
{
    // Before:                           After:
    //           ^
    //           |                       new
    // ----------|-----------> other     ---------->^-----------> other
    //           |                                  |
    //          road                               road

    // Create a new road segment
    m_branches.push_back(CityGenerator::Road(other));
    m_roads.push_back(&m_branches.back());
    CityGenerator::Road &new_road = m_branches.back();
    new_road.to = intersection;

    // Update state of the older roads
    road.to = intersection;
    road.has_severed = true;
    other.from = intersection;

    // Which links correspond to which end of the split segment
    if (other.isInversed())
    {
        // Before:                           After:
        //           ^
        //           |                       new
        // ----------|-----------> other     ---------->^-----------> other
        //           |                                  |
        //          road                               road

        // Create a new road segment

        for (auto &link : new_road.backwards)
        {
            auto it = std::find(link->backwards.begin(), link->backwards.end(), &other);
            if (it != link->backwards.end())
            {
                *it = &new_road;
            }
            else
            {
                it = std::find(link->forwards.begin(), link->forwards.end(), &other);
                assert(it != link->forwards.end());
                *it = &new_road;
            }
        }

        new_road.forwards.push_back(&road);
        new_road.forwards.push_back(&other);
        other.backwards.push_back(&road);
        other.backwards.push_back(&new_road);
        road.forwards.push_back(&new_road);
        road.forwards.push_back(&other);
    }
    else
    {
        // Before:                           After:
        //           ^
        //           |                       new
        // ----------|-----------> other     ---------->^-----------> other
        //           |                                  |
        //          road                               road

        // Create a new road segment

        for (auto &link : new_road.forwards)
        {
            auto it = std::find(link->forwards.begin(), link->forwards.end(), &other);
            if (it != link->forwards.end())
            {
                *it = &new_road;
            }
            else
            {
                it = std::find(link->backwards.begin(), link->backwards.end(), &other);
                assert(it != link->backwards.end());
                *it = &new_road;
            }
        }

        other.forwards.push_back(&road);
        other.forwards.push_back(&new_road);
        new_road.backwards.push_back(&road);
        new_road.backwards.push_back(&other);
        road.forwards.push_back(&other);
        road.forwards.push_back(&new_road);
    }
}

// -----------------------------------------------------------------------------
CityGenerator::CityGenerator()
{
    size_t priority = 0u;

    // Order rules by inverse of priority (hightest priority == lower value)
    // Be sure the priority value match with the index.
    m_rules.emplace_back(std::make_unique<DummyRule>(*this, priority++));
    m_rules.emplace_back(std::make_unique<RadiusIntersectionRule>(*this, priority++));
    m_rules.emplace_back(std::make_unique<SnapToCrossingRule>(*this, priority++));
    m_rules.emplace_back(std::make_unique<IntersectingRoadsRule>(*this, priority++));
}

// -----------------------------------------------------------------------------
std::list<CityGenerator::Road *> const &
CityGenerator::generate(sf::Vector2<Meter> const &dimension)
{
    // Reset internal states
    std::cout << "CityGenerator::generate clear" << std::endl;
    m_pendings.clear();
    m_roads.clear();
    m_branches.clear();

    //
    m_dimension = dimension;
    m_population.generate(dimension, sf::Vector2u(512u, 512u));
    m_population.save(project::info::tmp_path + "heatmap.png");
    std::cout << "CityGenerator::generate init roads" << std::endl;
    generateInitialRoads(sf::Vector2<Meter>(0.0_m, 0.0_m), true); // dimension.x / 2.0, dimension.y / 2.0), true);
    std::cout << "CityGenerator::generate algo" << std::endl;
    return generateRoads();
}

// -----------------------------------------------------------------------------
bool CityGenerator::localConstraints(CityGenerator::Road &road)
{
    size_t priority = 0u;
    size_t action = 0u;

    std::cout << "localConstraints " << road << std::endl;
    for (auto &other : m_roads)
    {
        std::cout << "  vs. Other " << *other << std::endl;
        size_t i = m_rules.size();
        while (i--)
        {
            if (priority <= m_rules[i]->priority)
            {
                if (m_rules[i]->accept(road, *other))
                {
                    priority = m_rules[i]->priority;
                    action = priority;
                }
            }
        }
    }

    return m_rules[action]->apply(road);
}

// -----------------------------------------------------------------------------
void CityGenerator::generateInitialRoads(sf::Vector2<Meter> const &initial_position,
                                         bool const highway)
{
    // Since we do not store initial roads inside \c m_roads and the graph structure
    // uses pointers we cannot use local variables.
    static std::vector<Road> m_initial_roads;
    m_initial_roads.clear();
    m_initial_roads.reserve(2u);

    // Create two initial roads with opposite direction and along the X axis. Indeed,
    // since the algorithm will make roads "growth" along their direction, this will
    // help spreading the city.
    //
    //        Init. Pos.
    //  <---------|--------->
    //     road1     road2
    const sf::Vector2<Meter> dx(config.highway_road_length, 0.0_m);
    // const sf::Vector2<Meter> dy(0.0_m, config.highway_road_length);
    const size_t priority = 0u;
    m_initial_roads.emplace_back(initial_position, initial_position + dx, priority, highway);
    m_initial_roads.emplace_back(initial_position, initial_position - dx, priority, highway);
    // m_initial_roads.emplace_back(initial_position, initial_position + dy, priority, highway);
    m_initial_roads[0].backwards.push_back(&m_initial_roads[1]);
    m_initial_roads[1].backwards.push_back(&m_initial_roads[0]);

    // Spread other roads form these initial roads.
    m_pendings.push(&m_initial_roads[0]);
    m_pendings.push(&m_initial_roads[1]);
}

// -----------------------------------------------------------------------------
std::list<CityGenerator::Road *> const &CityGenerator::generateRoads()
{
    while ((m_pendings.size() >= 1u) && (m_roads.size() < config.max_roads))
    {
        // Get the road with the hightest priority (lower value)
        Road &road = *m_pendings.pop();

        std::cout << "\n-----------------\n"
                  << road << std::endl;
        if (localConstraints(road))
        {
            road.setup_branch_links();
            m_roads.push_back(&road);
            globalGoals(road);
        }
    }

    return m_roads;
}

// -----------------------------------------------------------------------------
CityGenerator::Road
CityGenerator::continueRoad(CityGenerator::Road const &previous, Radian const direction)
{
    std::cout << "       segment_continue" << std::endl;
    size_t const priority = 0u;
    Meter const &l = previous.length();
    sf::Vector2<Meter> const to(previous.to.x + l * units::math::cos(direction),
                                previous.to.y + l * units::math::sin(direction));
    return Road(previous.to, to, priority, previous.highway); // FIXME severed ?
}

// -----------------------------------------------------------------------------
CityGenerator::Road
CityGenerator::branchRoad(CityGenerator::Road const &previous, Radian const direction)
{
    std::cout << "       segment_branch" << std::endl;
    const size_t priority = (previous.highway)
                                ? config.normal_branch_time_delay_from_highway
                                : 0u;
    Meter const &l = config.default_road_length;
    const sf::Vector2<Meter> to(previous.to.x + l * units::math::cos(direction),
                                previous.to.y + l * units::math::sin(direction));
    return Road(previous.to, to, priority, false); // Dummy meatadata
}

// -----------------------------------------------------------------------------
double CityGenerator::samplePopulation(Road const &road)
{
    // TODO: Along the way, samples of the population density are taken
    // from the population density map.
    // The population at every sample point on the ray is weighted with
    // the inverse distance to the roadend and summed up.
    return (m_population.get(road.from) + m_population.get(road.to)) / 2.0;
}

// -----------------------------------------------------------------------------
void CityGenerator::globalGoals(CityGenerator::Road &previous)
{
    std::cout << "global_goals_generate " << previous << std::endl;
    if (previous.has_severed)
    {
        std::cout << "       severed" << std::endl;
        return;
    }

    std::list<CityGenerator::Road> new_branches;

    // Continue the direction of the previous road
    const Road &&next_straight = continueRoad(previous, previous.heading());
    const double population_straight = samplePopulation(next_straight);
    if (previous.highway)
    {
        std::cout << "         highway" << std::endl;
        // Direction of the previous road straight with deviation
        const Road &&next_random = continueRoad(previous, previous.heading() + random_angle(config.straight_angle_deviation));
        const double population_random = samplePopulation(next_random);

        // Compare populations between the two choices. Select the
        // direction where the gradient of population density increases.
        double road_pop;
        std::cout << "         random pop " << population_random
                  << " straight pop " << population_straight
                  << std::endl;
        if (true) // population_random > population_straight)
        {
            std::cout << "         random selected" << std::endl;
            new_branches.push_back(next_random);
            road_pop = population_random;
        }
        else
        {
            std::cout << "         straight selected" << std::endl;
            new_branches.push_back(next_random);
            road_pop = population_straight;
        }

        // Turn the highway
        if (true) // road_pop > config.highway_branch_population_threshold)
        {
            // Left branch for the highway
            if (true) // Random::get<bool>(config.highway_branch_probability))
            {
                std::cout << "         left_highway_branch" << std::endl;
                const Radian angle = previous.heading() + 90.0_deg + random_angle(config.straight_angle_deviation);
                new_branches.push_back(continueRoad(previous, angle));
            }
            // Right branch for the highway
            else if (Random::get<bool>(config.highway_branch_probability))
            {
                std::cout << "         right_highway_branch" << std::endl;
                const Radian angle = previous.heading() - 90.0_deg + random_angle(config.straight_angle_deviation);
                new_branches.push_back(continueRoad(previous, angle));
            }
            else
            {
                std::cout << "         no highway branch" << std::endl;
            }
        }
    }
    else if (population_straight > config.normal_branch_population_threshold)
    {
        std::cout << "         straight pop" << std::endl;
        new_branches.push_back(next_straight);
    }

    // Turn the road
    if (true) // population_straight > config.normal_branch_population_threshold)
    {
        // Left branch for the road
        if (true) // Random::get<bool>(config.default_branch_probability))
        {
            std::cout << "         left_road_branch" << std::endl;
            const Radian angle = previous.heading() + 90.0_deg + random_angle(config.straight_angle_deviation);
            new_branches.push_back(branchRoad(previous, angle));
        }
        // Right branch for the road
        else if (Random::get<bool>(config.default_branch_probability))
        {
            std::cout << "         right_road_branch" << std::endl;
            const Radian angle = previous.heading() - 90.0_deg + random_angle(config.straight_angle_deviation);
            new_branches.push_back(branchRoad(previous, angle));
        }
        else
        {
            std::cout << "         no road branch" << std::endl;
        }
    }

    std::cout << "       new branches: " << std::endl;
    for (auto &branch : new_branches)
    {
        branch.previous_segment_to_link = &previous;
        branch.priority += previous.priority + 1u;
        m_branches.push_back(branch);
        std::cout << "          new branch: " << branch << std::endl;
        m_pendings.push(&m_branches.back());
    }
}