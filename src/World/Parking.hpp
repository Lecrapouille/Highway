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

    Parking(ParkingDimension const& d, sf::Vector2f const& position, Car& car);
    Parking(ParkingDimension const& d, sf::Vector2f const& position);
    void bind(Car& car);
    void unbind();
    bool empty() const;
    Car& car();
    sf::Vector2f const& position() const { return m_position; }

public:

    ParkingDimension const dim;

private:

    Car* m_car = nullptr;
    sf::Vector2f m_position;
};

#endif
