//=====================================================================
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
//=====================================================================

#ifndef ROAD_HPP
#  define ROAD_HPP

#  include "Simulator/Actor.hpp"
#  include "Math/Units.hpp"
#  include <SFML/Graphics/RectangleShape.hpp>
#  include <list>
#  include <vector>
#  include <memory>

// https://fr.mathworks.com/help/driving/ref/drivingscenario.road.html

#define COLOR_DRIVING_LANE sf::Color(204, 204, 204)
#define COLOR_RESTRICTED_LANE sf::Color(150, 143, 158)
#define COLOR_PARKING_LANE sf::Color(71, 71, 71)

class Car;

enum TrafficSide { LeftHand, RightHand, Max };

// *************************************************************************
//! \brief Class holding lane slot dimensions
// *************************************************************************
struct LaneBluePrint
{
    //----------------------------------------------------------------------
    //! \brief Set road segment dimensions.
    //! \param[in] l road length [meter].
    //! \param[in] w road width [meter].
    //! \param[in] a road lane angle [deg].
    //----------------------------------------------------------------------
    LaneBluePrint(Meter const l, Meter const w, Radian const a);

    //! \brief Vehicle length [meter]
    Meter length;
    //! \brief Vehicle width [meter]
    Meter width;
    //! \brief Orientation [rad]
    Radian angle;
};

// ****************************************************************************
//! \brief
// ****************************************************************************
class Lane
{
public:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    Lane(sf::Vector2<Meter> const& start, sf::Vector2<Meter> const& stop,
         Meter const width, TrafficSide side);

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    Lane(Lane const& other)
        : blueprint(other.blueprint), side(other.side), m_cars(other.m_cars)
    {}

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    Lane& operator=(Lane const& other)
    {
        this->~Lane(); // destroy
        new (this) Lane(other); // copy construct in place
        return *this;
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    sf::RectangleShape const& shape() const
    {
        return m_shape;
    }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    sf::Vector2<Meter> position() const
    {
        sf::Vector2f p(m_shape.getPosition());
        return { Meter(p.x), Meter(p.y) };
    }

public:

    LaneBluePrint const blueprint;
    TrafficSide side;
    std::vector<Car*> m_cars;

private:

    //! \brief Oriented bounding box for attitude and collision
    sf::RectangleShape m_shape;
};

// ****************************************************************************
//! \brief
// ****************************************************************************
class Road: public StaticActor // TODO https://www.youtube.com/watch?v=tHXIwijaERg
{
public:

    //--------------------------------------------------------------------------
    //! \brief Empty road segment with given ways.
    //! \param[in] blueprint: parking slot dimension.
    //! \param[in] position: position in the world coordinates of the middle of
    //! the left side lane.
    //! \param[in] lanes nuber of lanes for right- and left-hand drive.
    //--------------------------------------------------------------------------
    Road(std::vector<sf::Vector2<Meter>> const& centers,
         Meter const width, std::array<size_t, TrafficSide::Max> const& lanes);

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    sf::Vector2<Meter> offset(TrafficSide const side, size_t const lane,
                              double const x, double const y);

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    inline sf::Vector2<Meter> start() const { return m_start; }

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    inline sf::Vector2<Meter> stop() const { return m_stop; }

#if 0
    //--------------------------------------------------------------------------
    //! \brief Make the slot occupied by the car.
    //! \param[inout] car to be parked. Its position and orientation will be
    //! modified by this method to follow the parking slot. Use this method only
    //! when constructing the simulation world.
    //--------------------------------------------------------------------------
    //bool bind(Car& car, size_t const way, std::Vector2<Meter> const& offset);

    //-------------------------------------------------------------------------
    //! \brief Const getter: return the oriented bounding box of the parking.
    //-------------------------------------------------------------------------
    inline Radian heading() const
    {
        return blueprint.angle;
    }

    //--------------------------------------------------------------------------
    //! \brief Get Cars on the road.
    //--------------------------------------------------------------------------
    std::list<Car*> cars(size_t const d, size_t const way);

#endif

private:

    //! \brief Initial center position of the road
    sf::Vector2<Meter> m_start;
    //! \brief Final center position of the road
    sf::Vector2<Meter> m_stop;
    //! \brief Width for each lanes
    Meter m_width;

public:

    //! \brief Right side lanes and left side lanes
    std::vector<std::unique_ptr<Lane>> m_lanes[TrafficSide::Max];
};

#endif
