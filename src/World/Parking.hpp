// 2021 Quentin Quadrat lecrapouille@gmail.com
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

#  include "ParkingDimension.hpp"
#  include <SFML/System/Vector2.hpp>
#  include <cassert>

class Car;

class Parking
{
public:

    enum Type { Parallel, Perpendicular, Diagonal45, Diagonal60, Diagonal75 };

    //! \brief Parking slot holding a parked car.
    //! \param[in] d: parking slot dimension
    //! \param[in] position: position of the middle of the left side.
    //! \param[inout] car to be parked. Its position and orientation will be modified
    //! by this method.
    //! \note the parking local orientation is providen by ParkingDimension.
    //! \note we do not manage the orientation in the world coordinate.
    Parking(ParkingDimension const& d, sf::Vector2f const& position, Car& car);

    //! \brief Empty parking.
    //! \param[in] d: parking slot dimension
    //! \param[in] position: position of the middle back of the slot.
    //! \note the parking local orientation is providen by ParkingDimension.
    //! \note we do not manage the orientation in the world coordinate.
    Parking(ParkingDimension const& d, sf::Vector2f const& position);

    //! \brief Make the slot occupied by the car.
    //! \param[inout] car to be parked. Its position and orientation will be modified
    //! by this method.
    void bind(Car& car);

    //! \brief Set the parking slot empty. The parked car stay at its current position.
    void unbind();

    //! \brief Is the parking occupied by a parked car ?
    bool empty() const;

    //! \brief Parked car getter (non const).
    Car& car();

    //! \brief Position getter (non const) of the middle of the left side.
    sf::Vector2f const& position() const { return m_position; }

    //! \brief Delta position to place the next parking
    sf::Vector2f delta() const
    {
        sf::Vector2f p = ROTATE(sf::Vector2f(dim.length, dim.width), -dim.angle);
        return sf::Vector2f(p.x, 0.0f); // W
    }

public:

    //! \brief Dimension and information
    ParkingDimension const dim;
    Parking::Type const type;

private:

    Car* m_car = nullptr;
    sf::Vector2f m_position;
};

#endif
