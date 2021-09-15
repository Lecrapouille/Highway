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

#ifndef CAR_CONTROL_HPP
#  define CAR_CONTROL_HPP

class CarControl
{
public:

    void set()
    {}

    //----------------------------------------------------------------------
    //! \brief The desired
    //----------------------------------------------------------------------
    struct References
    {
        float body_speed = 0.0f;
        float steering = 0.0f;
    };

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    struct Inputs
    {
    };

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    struct Outputs
    {
        float body_speed = 0.0f;
        float steering = 0.0f;
    };

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_speed(float const speed)
    {
        ref.body_speed = speed;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_steering(float const steering)
    {
        ref.steering = steering;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_acceleration(float const acc, float const dt)
    {
        ref.body_speed += acc * dt;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_steering_speed(float const steering, float const dt)
    {
        ref.steering += steering * dt;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void update(float const /*dt*/ /*, sensors*/)
    {
        //
        outputs.body_speed = ref.body_speed;
        outputs.steering = ref.steering;
    }

    References ref;
    Outputs outputs;
};

#endif
