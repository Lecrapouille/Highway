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

#ifndef CAR_DIMENSION_HPP
#  define CAR_DIMENSION_HPP

#  include "Utils.hpp"
#  include <ostream>
#  include <cassert>

struct TrailerDimension
{
    TrailerDimension(const float w, const float l, const float l2,
                     const float bo, const float wr)
        : width(w), width2(0.1f), length(l), wheelbase(l2),  back_overhang(bo),
          wheel_radius(wr)
    {}

    //! \brief Vehicle width [meter]
    float width, width2;
    //! \brief Vehicle length [meter]
    float length, wheelbase;
    //! \brief Porte a faux arriere [meter]
    float back_overhang;
    //! \brief Rayon roue [meter]
    float wheel_radius;
    //! \brief Epaisseur roue [meter] (only used for the rendering)
    float wheel_width = 0.1f;
};

struct CarDimension
{
    //--------------------------------------------------------------------------
    //! \brief Define vehicle constants
    //! \param[in] w: car width [meter]
    //! \param[in] l: car length [meter]
    //! \param[in] wb: wheelbase length [meter]
    //! \param[in] bo: back overhang [meter]
    //! \param[in] wr: wheel radius [meter]
    //! \param[in] tc: turning circle [meter]
    //--------------------------------------------------------------------------
    CarDimension(const float w, const float l, const float wb,
                 const float bo, const float wr, const float /*tc*/)
        : width(w), length(l), wheelbase(wb), back_overhang(bo),
          front_overhang(length - wheelbase - back_overhang), wheel_radius(wr)
    {
        assert(max_steering_angle > 0.0f);
        assert(max_steering_angle < DEG2RAD(90.0f));
    }

    friend std::ostream& operator<<(std::ostream& os, CarDimension const& dim)
    {
        return os << "  Dim = {" << std::endl
                  << "    length=" << dim.length << std::endl
                  << "    width=" << dim.width << std::endl
                  << "    wheelbase=" << dim.wheelbase << std::endl
                  << "    back_overhang=" << dim.back_overhang << std::endl
                  << "    front_overhang=" << dim.front_overhang << std::endl
                  << "  }";
    }

    //! \brief Vehicle width [meter]
    float width;
    //! \brief Vehicle length [meter]
    float length;
    //! \brief Empattement [meter]
    float wheelbase;
    //! \brief Porte a faux arriere [meter]
    float back_overhang;
    //! \brief Porte a faux avant [meter]
    float front_overhang;
    //! \brief Rayon roue [meter]
    float wheel_radius;
    //! \brief Epaisseur roue [meter] (only used for the rendering)
    float wheel_width = 0.1f;
    //! \brief Limit of steering angle absolute angle [rad]
    float max_steering_angle = DEG2RAD(35.0f); // FIXME
};

#endif
