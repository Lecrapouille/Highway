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

// -----------------------------------------------------------------------------
static double non_linear_distribution(double const limit)
{
    // Non-linear distribution
    double const non_uniform_norm = limit * limit * limit;
    double val;
    do {
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
void CityGenerator::Road::setup_branch_links()
{
    if (previous_segment_to_link == nullptr)
        return ;

    for (auto& link: previous_segment_to_link->backwards)
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
void CityGenerator::junction(CityGenerator::Road& road, CityGenerator::Road& other,
                             sf::Vector2<Meter>& intersection)
{
    // Create a new road segment
    m_roads.push_back(CityGenerator::Road(other));
    CityGenerator::Road& split_part = m_roads.back();
    split_part.to = intersection;

    // Update state of the older roads
    road.to = intersection;
    road.has_severed = true;
    other.from = intersection;

    // Which links correspond to which end of the split segment
    if (true)
    {
        for (auto& link: split_part.backwards)
        {
            auto it = std::find(link->backwards.begin(), link->backwards.end(), &other);
            if (it != link->backwards.end())
            {
                *it = &split_part;
            }
            else
            {
                it = std::find(link->forwards.begin(), link->forwards.end(), &other);
                assert(it != link->forwards.end());
                *it = &split_part;
            }
        }

        split_part.forwards.push_back(&road);
        split_part.forwards.push_back(&other);
        other.backwards.push_back(&road);
        other.backwards.push_back(&split_part);
        road.forwards.push_back(&other);
        road.forwards.push_back(&split_part);
    }
    else
    {

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
CityGenerator::Roads const&
CityGenerator::generate(sf::Vector2<Meter> const& dimension)
{
    PriorityQueue empty;
    m_pendings.swap(empty);
    m_roads.clear();
    m_new_branches.clear();

    generatePopulationMap(dimension);
    exportPopulationMap(project::info::tmp_path + "heatmap.png");
    generateInitialRoads(sf::Vector2<Meter>(0.0_m, 0.0_m), true);
        //dimension.x / 2.0, dimension.y / 2.0), false);
    return generateRoads();
}

// -----------------------------------------------------------------------------
void CityGenerator::generatePopulationMap(sf::Vector2<Meter> const& dimension)
{
    sf::Vector2u dim(static_cast<unsigned int>(dimension.x),
                     static_cast<unsigned int>(dimension.y));
    perlin(m_heatmap, dim, [](double const x, double const y) -> sf::Color {
        double const dt = 1.0;
        auto const noise = (
            db::perlin(x / 64.0, y / 64.0, dt * 0.25) * 1.0 +
            db::perlin(x / 32.0, y / 32.0, dt * 0.75) * 0.5
        ) / 1.5;

        auto const brightness = sf::Uint8((noise * 0.5 + 0.5) * 255.0);
        // TODO Idea: store 3 other maps (water, park, elevation, pedestrians)
        return { brightness, brightness, brightness, 255 };
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
double CityGenerator::population(sf::Vector2<Meter> const position)
{
    // FIXME: convert world position to map position
    const unsigned int u = static_cast<unsigned int>(position.x.value());
    const unsigned int v = static_cast<unsigned int>(position.y.value());

    std::cout << "Population at " << position.x << ", " << position.y << std::endl;

    return 0.0;//double(m_heatmap.getPixel(u, v).r);
}

// -----------------------------------------------------------------------------
bool CityGenerator::localConstraints(CityGenerator::Road& road)
{
    size_t priority = 0u;
    size_t action = 0u;

    std::cout << "localConstraints Road " << road << std::endl;
    for (auto& other: m_roads)
    {
        std::cout << "  vs. Other " << other << std::endl;
        for (auto& rule: m_rules)
        {
            if (priority <= rule->priority)
            {
                if (rule->accept(road, other))
                {
                    priority = rule->priority;
                    action = priority;
                }
            }
        }
    }

    return m_rules[action]->apply(road);
}

// -----------------------------------------------------------------------------
void CityGenerator::generateInitialRoads(sf::Vector2<Meter> const& initial_position,
                                         bool const highway)
{
    // Since we do not store initial roads inside \c m_roads and the graph structure
    // uses pointers we cannot use local variables.
    static std::vector<Road> m_initial_roads;
    m_initial_roads.clear();

    // Create two initial roads with opposite direction and along the X axis. Indeed,
    // since the algorithm will make roads "growth" along their direction, this will
    // help spreading the city.
    //
    //        Init. Pos.
    //  <---------|--------->
    //     road1     road2
    const sf::Vector2<Meter> dx(config.highway_road_length, 0.0_m);
    m_initial_roads.emplace_back(initial_position, initial_position + dx, 0, highway);
    m_initial_roads.emplace_back(initial_position, initial_position - dx, 0, highway);
    m_initial_roads[0].backwards.push_back(&m_initial_roads[1]);
    m_initial_roads[1].backwards.push_back(&m_initial_roads[0]);

    // Spread other roads form these initial roads.
    m_pendings.push(&m_initial_roads[0]);
    m_pendings.push(&m_initial_roads[1]);
}

// -----------------------------------------------------------------------------
CityGenerator::Roads const& CityGenerator::generateRoads()
{
    while ((m_pendings.size() >= 1u) && (m_roads.size() < config.max_roads))
    {
        // Get the road with the hightest priority (lower value)
        Road& road = *m_pendings.top();
        m_pendings.pop();

        std::cout << "Road " << road << std::endl;
        if (localConstraints(road))
        {
            road.setup_branch_links();
            m_roads.push_back(road);
            globalGoals(road);
        }
    }

    return m_roads;
}

// -----------------------------------------------------------------------------
CityGenerator::Road
CityGenerator::continueRoad(CityGenerator::Road const& previous, Radian const direction)
{
    size_t const priority = 0u;
    Meter const& l = previous.length();
    sf::Vector2<Meter> const to(previous.to.x + l * units::math::sin(direction),
                                previous.to.y + l * units::math::cos(direction));
    return Road(previous.from, to, priority, previous.highway); // FIXME severed ?
}

// -----------------------------------------------------------------------------
CityGenerator::Road
CityGenerator::branchRoad(CityGenerator::Road const& previous, Radian const direction)
{
    const size_t priority = (previous.highway)
                          ? config.normal_branch_time_delay_from_highway
                          : 0u;
    Meter const& l = config.default_road_length;
    const sf::Vector2<Meter> to(previous.to.x + l * units::math::sin(direction),
                                previous.to.y + l * units::math::cos(direction));
    return Road(previous.from, to, priority, false); // Dummy meatadata
}

// -----------------------------------------------------------------------------
double CityGenerator::samplePopulation(Road const& road)
{
    // TODO: Along the way, samples of the population density are taken
    // from the population density map.
    // The population at every sample point on the ray is weighted with
    // the inverse distance to the roadend and summed up.
    return (population(road.from) + population(road.to)) / 2.0;
}

// -----------------------------------------------------------------------------
void CityGenerator::globalGoals(CityGenerator::Road& previous)
{
    std::cout << "    global_goals_generate " << previous << std::endl;
    if (previous.has_severed)
        return ;

    std::cout << "       severed" << std::endl;
    auto it = m_new_branches.end();

    // Continue the direction of the previous road
    const Road next_straight = continueRoad(previous, previous.heading());
    const double population_straight = samplePopulation(next_straight);
    if (previous.highway)
    {
        std::cout << "         highway" << std::endl;
        // Direction of the previous road straight with deviation
        const Road next_random = continueRoad(previous, previous.heading()
                         + random_angle(config.straight_angle_deviation));
        const double population_random = samplePopulation(next_random);

        // Compare populations between the two choices. Select the
        // direction where the gradient of population density increases.
        double road_pop;
        if (population_random > population_straight)
        {
            m_new_branches.push_back(next_random);
            road_pop = population_random;
        }
        else
        {
            m_new_branches.push_back(next_straight);
            road_pop = population_straight;
        }

        // Turn the higway
        if (road_pop > config.highway_branch_population_threshold)
        {
            // Left branch for the highway
            if (Random::get<bool>(config.highway_branch_probability))
            {
                const Radian angle = previous.heading() - 90.0_deg + random_angle(config.straight_angle_deviation);
                m_new_branches.push_back(continueRoad(previous, angle));
            }
            // Right branch for the highway
            else if (Random::get<bool>(config.highway_branch_probability))
            {
                const Radian angle = previous.heading() + 90.0_deg + random_angle(config.straight_angle_deviation);
                m_new_branches.push_back(continueRoad(previous, angle));
            }
        }
    }
    else if (population_straight > config.normal_branch_population_threshold)
    {
        m_new_branches.push_back(next_straight);
    }

    // Turn the road
    if (population_straight > config.normal_branch_population_threshold)
    {
        // Left branch for the road
        if (Random::get<bool>(config.default_branch_probability))
        {
            const Radian angle = previous.heading() - 90.0_deg + random_angle(config.straight_angle_deviation);
            m_new_branches.push_back(continueRoad(previous, angle));
        }
        // Right branch for the road
        else if (Random::get<bool>(config.default_branch_probability))
        {
            const Radian angle = previous.heading() + 90.0_deg + random_angle(config.straight_angle_deviation);
            m_new_branches.push_back(continueRoad(previous, angle));
        }
    }

    for (auto& branch = it; branch != m_new_branches.end(); ++branch)
    {
        std::cout << "       new branch: " << *branch << std::endl;
        branch->previous_segment_to_link = &previous;
        branch->priority += previous.priority + 1u;
        m_pendings.push(&(*branch));
    }
}

/*
int main()
{
    CityGenerator g;
    g.create(sf::Vector2u(800u, 600u));

    std::vector<Road> const& roads = g.generateRoads(sf::sf::Vector2f(0.0f, 0.0f), sf::sf::Vector2f(100.0f, 0.0f), 10u);
    for (auto const& it: roads)
    {
        std::cout << "Road (" << it.from.x << ", " << it.from.y << "), ("
                  << it.to.x << ", " << it.to.y << ")" << std::endl;
    }

    return 0;
}
*/
