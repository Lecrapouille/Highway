// 2021 Quentin Quadrat quentin.quadrat@gmail.com
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

#ifndef PARKING_HPP
#  define PARKING_HPP

#  include "World/ParkingDimension.hpp"
#  include <SFML/System/Vector2.hpp>
#  include <ostream>
#  include <cassert>
#  include <new>

class Car;

// ****************************************************************************
//! \brief Class defining a parking slot as entity in the simulation world which
//! can interact with other entities such as vehicles to give some helper
//! methods for creating the simulation more easily (ie creating a parking
//! world, place parked cars, get spot coordinates ...)
// ****************************************************************************
class Parking
{
public:

    //! \brief Kind of parking
    enum Type { Parallel, Perpendicular, Diagonal45, Diagonal60, Diagonal75 };

    //--------------------------------------------------------------------------
    //! \brief Parking slot holding a parked car.
    //! \param[in] d: parking slot dimension.
    //! \param[in] position: position in the world coordinates of the middle of
    //! the left side lane.
    //! \param[inout] car to be parked inside this parking. The car's position
    //! and orientation will be modified by this method to follow the parking
    //! slot.
    //! \note the parking lane orientation is providen by ParkingDimension.
    //! \note we do not manage the orientation in the world coordinate.
    //--------------------------------------------------------------------------
    Parking(ParkingDimension const& d, sf::Vector2f const& position, Car& car);

    //--------------------------------------------------------------------------
    //! \brief Empty parking slot.
    //! \param[in] d: parking slot dimension
    //! \param[in] position: position in the world coordinates of the middle of
    //! the left side lane.
    //! \note the parking local orientation is providen by ParkingDimension.
    //! \note we do not manage the orientation in the world coordinate.
    //--------------------------------------------------------------------------
    Parking(ParkingDimension const& d, sf::Vector2f const& position);

    //--------------------------------------------------------------------------
    //! \brief Copy operator. Ugly code needed because of const member vriables.
    //--------------------------------------------------------------------------
    Parking& operator=(const Parking& obj)
    {
        this->~Parking(); // destroy
        new (this) Parking(obj.dim, obj.position()); // copy construct in place
        return *this;
    }

    //--------------------------------------------------------------------------
    //! \brief Needed because implicit copy constructor is deprecated.
    //--------------------------------------------------------------------------
    Parking(Parking const& other)
        : Parking(other.dim, other.position())
    {}

    //--------------------------------------------------------------------------
    //! \brief Make the slot occupied by the car.
    //! \param[inout] car to be parked. Its position and orientation will be
    //! modified by this method to follow the parking slot. Use this method only
    //! when constructing the simulation world.
    //--------------------------------------------------------------------------
    void bind(Car& car);

    //--------------------------------------------------------------------------
    //! \brief Set the parking slot empty. If a car was already parked it stays
    //! at its current position.
    //--------------------------------------------------------------------------
    void unbind();

    //--------------------------------------------------------------------------
    //! \brief Is the parking occupied by a parked car ?
    //--------------------------------------------------------------------------
    bool empty() const;

    //--------------------------------------------------------------------------
    //! \brief Parked car getter (non const).
    //--------------------------------------------------------------------------
    Car& car();

    //--------------------------------------------------------------------------
    //! \brief Const position in the world coordinates of the middle of the left
    //! lane. Once placed in the world the parking slot cannot be displaced.
    //--------------------------------------------------------------------------
    sf::Vector2f const& position() const
    {
        return m_position;
    }

    //--------------------------------------------------------------------------
    //! \brief Helper method to place the next parking along the X-axis.
    //--------------------------------------------------------------------------
    sf::Vector2f delta() const
    { // FIXME not working for diagnonal spot
        sf::Vector2f p = HEADING(sf::Vector2f(dim.length, dim.width), -dim.angle);
        return sf::Vector2f(p.x, 0.0f); // W
    }

    //--------------------------------------------------------------------------
    //! \brief Debug purpose only: show shape information.
    //--------------------------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, Parking const& parking)
    {
        os << "Parking P = (" << parking.m_position.x << ", " << parking.m_position.y << "), "
           << "length = " << parking.dim.length << ", width = " << parking.dim.width
           << ", angle = " << parking.dim.deg;
        return os;
    }

public:

    //! \brief Slot dimension.
    ParkingDimension const dim;
    //! \brief Type of slot.
    Parking::Type const type;

protected:

    //! \brief Position inside the World coordinate
    sf::Vector2f m_position;

private:

    //! \brief Empty/Occupied slot?
    Car* m_car = nullptr;
};

#endif
