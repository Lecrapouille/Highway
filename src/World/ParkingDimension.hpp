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

#ifndef PARKING_DIMENSION_HPP
#  define PARKING_DIMENSION_HPP

#  include "Utils/Utils.hpp"

// ****************************************************************************
//! \brief Class holding parking slot dimensions
// ****************************************************************************
struct ParkingDimension
{
    //--------------------------------------------------------------------------
    //! \brief Set parking slot dimensions.
    //! \param[in] l parking length.
    //! \param[in] w parking width.
    //! \param[in] a parking lane angle (0°: parallel, 90°: perpendicular).
    //--------------------------------------------------------------------------
    ParkingDimension(float const l, float const w, size_t const a)
        : length(l), width(w), angle(DEG2RAD(float(a))), deg(a)
    {}

    //! \brief Vehicle length [meter]
    float length;
    //! \brief Vehicle width [meter]
    float width;
    //! \brief Orientation [rad]
    float angle;
    //! \brief Orientation [deg]
    size_t deg;
};

#endif
