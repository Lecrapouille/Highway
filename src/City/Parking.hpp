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

#ifndef PARKING_HPP
#  define PARKING_HPP

#  include "Math/Math.hpp"
#  include <SFML/Graphics/RectangleShape.hpp>
#  include <ostream>
#  include <cassert>
#  include <new>
#  include <map>

class Car;

// *************************************************************************
//! \brief Class holding parking slot dimensions
// *************************************************************************
struct ParkingBluePrint
{
    //----------------------------------------------------------------------
    //! \brief Set parking slot dimensions.
    //! \param[in] l parking length [meter].
    //! \param[in] w parking width [meter].
    //! \param[in] a parking lane angle [deg] (0°: parallel, 90°:
    //! perpendicular).
    //----------------------------------------------------------------------
    ParkingBluePrint(Meter const l, Meter const w, Degree const a)
        : length(l), width(w), angle(a)
    {}

    //! \brief Vehicle length [meter]
    Meter length;
    //! \brief Vehicle width [meter]
    Meter width;
    //! \brief Orientation [deg]
    Degree angle;
};

// ****************************************************************************
//! \brief Class defining a parking slot as entity in the simulation world which
//! can interact with other entities such as vehicles to give some helper
//! methods for creating the simulation more easily (ie creating a parking
//! world, place parked cars, get spot coordinates ...)
// ****************************************************************************
class Parking //: public StaticActor // FIXME add ParkingShape like done with Sensor
{
public:

    //--------------------------------------------------------------------------
    //! \brief Kind of parking
    //! ../../doc/pics/PerpendicularSpots.gif
    //! ../../doc/pics/DiagonalSpots.gif
    //! ../../doc/pics/ParallelSpots.jpg
    //--------------------------------------------------------------------------
    enum Type { Parallel, Perpendicular, Diagonal45, Diagonal60, Diagonal75 };

    //--------------------------------------------------------------------------
    //! \brief Parking slot holding a parked car.
    //! \param[in] blueprint: parking slot dimension.
    //! \param[in] position: position in the world coordinates of the middle of
    //! the left side lane.
    //! \param[inout] car to be parked inside this parking. The car's position
    //! and orientation will be modified by this method to follow the parking
    //! slot.
    //! \note the parking lane orientation is providen by ParkingBluePrint.
    //! \note we do not manage the orientation in the world coordinate.
    //--------------------------------------------------------------------------
    Parking(ParkingBluePrint const& blueprint, sf::Vector2<Meter> const& position,
            Radian const heading, Car& car);

    //--------------------------------------------------------------------------
    //! \brief Empty parking slot.
    //! \param[in] d: parking slot dimension
    //! \param[in] position: position in the world coordinates of the middle of
    //! the left side lane.
    //! \note the parking local orientation is providen by ParkingBluePrint.
    //! \note we do not manage the orientation in the world coordinate.
    //--------------------------------------------------------------------------
    Parking(ParkingBluePrint const& d, sf::Vector2<Meter> const& position,
            Radian const heading);

    //--------------------------------------------------------------------------
    //! \brief Copy operator. Ugly code needed because of const member vriables.
    //--------------------------------------------------------------------------
    Parking& operator=(const Parking& other)
    {
        // destroy
        this->~Parking();
        // copy construct in place
        new (this) Parking(other.blueprint, other.position(), other.heading() /*, other.m_car*/);
        return *this;
    }

    //--------------------------------------------------------------------------
    //! \brief Needed because implicit copy constructor is deprecated.
    //--------------------------------------------------------------------------
    Parking(Parking const& other)
        : Parking(other.blueprint, other.position(), other.heading())
    {}

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void update(Second const dt);

    //--------------------------------------------------------------------------
    //! \brief Place a vehicle in the parking slot. The slot will be set
    //! occupied and the car position and orientation will be modified to follow
    //! the parking slot orientation. You should only call this method during
    //! the simulation initialization for creating the city (and during the
    //! simulation but in an off-rendered part of the city) since there is not
    //! physcally realist.
    //! \param[inout] car to be parked.
    //--------------------------------------------------------------------------
    bool bind(Car& car);

    //--------------------------------------------------------------------------
    //! \brief Is the parking occupied by a parked car ?
    //--------------------------------------------------------------------------
    inline bool empty() const { return m_car == nullptr; }

    //--------------------------------------------------------------------------
    //! \brief Parked car getter (non const).
    //--------------------------------------------------------------------------
    inline Car& car() { assert(m_car != nullptr); return *m_car; }

    //--------------------------------------------------------------------------
    //! \brief Return the origin position of the parking (middle of the left
    //! side) inside the world coordinate [Meter].
    //! \note Once placed in the world the parking slot cannot be displaced.
    //--------------------------------------------------------------------------
    sf::Vector2<Meter> origin() const
    {
        return position() + math::heading(
            sf::Vector2<Meter>(0.0_m, -blueprint.width / 2.0),
            blueprint.angle - m_heading);
    }

    //--------------------------------------------------------------------------
    //! \brief Return the top-left position of the parking inside the world
    //! coordinate [Meter].
    //! \note Once placed in the world the parking slot cannot be displaced.
    //--------------------------------------------------------------------------
    sf::Vector2<Meter> position() const
    {
        sf::Vector2f p(m_shape.getPosition());
        return { Meter(p.x), Meter(p.y) };
    }

    //-------------------------------------------------------------------------
    //! \brief Const getter: return the oriented bounding box of the parking.
    //-------------------------------------------------------------------------
    inline Radian const& heading() const { return m_heading; }

    //-------------------------------------------------------------------------
    //! \brief Const getter: return the oriented bounding box of the parking.
    //-------------------------------------------------------------------------
    inline sf::RectangleShape const& obb() const { return m_shape; }

    //--------------------------------------------------------------------------
    //! \brief Helper method to place the next parking along the X-axis.
    //--------------------------------------------------------------------------
    sf::Vector2<Meter> delta() const
    {
        return position() + math::heading(
            sf::Vector2<Meter>(blueprint.length, 0.0_m),
            blueprint.angle - m_heading);
    }

    //--------------------------------------------------------------------------
    //! \brief Debug purpose only: show shape information.
    //--------------------------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, Parking const& parking)
    {
        os << "Parking P = ("
           << parking.position().x << ", "
           << parking.position().y << "), "
           << "length = " << parking.blueprint.length
           << ", width = " << parking.blueprint.width
           << ", angle = " << parking.blueprint.angle;
        return os;
    }

private:

    //--------------------------------------------------------------------------
    //! \brief Set the slot occupied by the given car.
    //! \param[inout] car to be parked.
    //--------------------------------------------------------------------------
    bool setOccupied(Car& car);

public:

    //! \brief Slot dimension.
    ParkingBluePrint const blueprint;
    //! \brief Type of slot.
    Parking::Type const type;
    //! \brief
    sf::Color initial_color = sf::Color(71, 71, 71);
    //! \brief
    sf::Color color = sf::Color(71, 71, 71);

protected:

    //! \brief Oriented bounding box for attitude and collision
    sf::RectangleShape m_shape;
    //! \brief Heading [rad]
    Radian m_heading;
    //! \brief Empty/Occupied slot?
    Car* m_car = nullptr;
};

#endif
