// 2021 -- 2022 Quentin Quadrat quentin.quadrat@gmail.com
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

#ifndef VEHICLE_BLUEPRINT_HPP
#  define VEHICLE_BLUEPRINT_HPP

#  include <SFML/Graphics.hpp>
#  include <array>

// *************************************************************************
//! \brief Wheel blueprint.
// *************************************************************************
struct WheelBluePrint
{
    //! \brief Relative position from the car shape position (middle rear axle)
    sf::Vector2f offset;
    //! \brief current position and altitude inside the world coordinate.
    sf::Vector2f position; // FIXME to be moved outside + 3D
    //! \brief Rayon roue [meter]
    float radius;
    //! \brief Epaisseur roue [meter] (only used for the rendering)
    float width;
};

// *************************************************************************
//! \brief Car blueprint. See picture ../../doc/pics/CarDefinition.png
// *************************************************************************
struct CarBluePrint
{
    //--------------------------------------------------------------------------
    //! \brief Wheel's names: FL: front left, FR: front rigeht, RR: rear right,
    //! RL: rear left.
    //--------------------------------------------------------------------------
    enum WheelName { RR, RL, FL, FR, MAX };

    //----------------------------------------------------------------------
    //! \brief Define vehicle constants:
    //! See ../../doc/pics/CarDefinition.png
    //! \param[in] l: car length [meter]
    //! \param[in] w: car width [meter]
    //! \param[in] wb: wheelbase length [meter]
    //! \param[in] bo: back overhang [meter]
    //! \param[in] wr: wheel radius [meter]
    //! \param[in] tc: turning diameter [meter]
    //----------------------------------------------------------------------
    CarBluePrint(const float l, const float w, const float wb, const float bo,
                 const float wr, const float td);

    //! \brief Vehicle length [meter]
    float length;
    //! \brief Vehicle width [meter]
    float width;
    //! \brief Wheel to wheel distance along width [meter]
    float track;
    //! \brief Wheel to wheel distance along the length [meter]
    float wheelbase;
    //! \brief Porte a faux arriere [meter]
    float back_overhang;
    //! \brief Porte a faux avant [meter]
    float front_overhang;
    //! \brief Limit of control.outputs.steering angle absolute angle [rad]
    float max_steering_angle;
    //! \brief Blue prints for the wheels
    std::array<WheelBluePrint, WheelName::MAX> wheels;
};

// *****************************************************************************
//! \brief Trailer blueprint
// *****************************************************************************
struct TrailerBluePrint
{
    //--------------------------------------------------------------------------
    //! \brief Wheel's names: RR: rear right, RL: rear left.
    //--------------------------------------------------------------------------
    enum WheelName { RR, RL, MAX };

    //--------------------------------------------------------------------------
    //! \brief Define trailer constants
    //! \param[in] l: trailer length [meter]
    //! \param[in] w: trailer width [meter]
    //! \param[in] wb: wheelbase length (fork length) [meter]
    //! \param[in] bo: back overhang [meter]
    //! \param[in] wr: wheel radius [meter]
    //--------------------------------------------------------------------------
    TrailerBluePrint(const float l, const float w, const float d,
                     const float bo, const float wr);

    //! \brief Vehicle length [meter]
    float length;
    //! \brief Vehicle width [meter]
    float width;
    //! \brief Wheel to wheel distance along width [meter]
    float track;
    //! \brief Wheel to wheel distance along the length [meter]
    float wheelbase;
    //! \brief Porte a faux arriere [meter]
    float back_overhang;
    //! \brief Width of the fork [meter] (only used for the rendering)
    float fork_width = 0.1f;
    //! \brief Blue prints for the wheels
    std::array<WheelBluePrint, WheelName::MAX> wheels;
};

#endif
