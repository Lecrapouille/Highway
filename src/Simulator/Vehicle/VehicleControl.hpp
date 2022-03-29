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

#ifndef VEHICLE_CONTROL_HPP
#  define VEHICLE_CONTROL_HPP

class VehicleControl
{
public:

    //----------------------------------------------------------------------
    //! \brief The desired values
    //----------------------------------------------------------------------
    struct References
    {
        float speed = 0.0f;
        float steering = 0.0f;
    };

    //----------------------------------------------------------------------
    //! \brief The desired values
    //----------------------------------------------------------------------
    struct Inputs
    {
        float throttle = 0.0f; // [%]
        float brake = 0.0f; // [%]
    };

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    struct Outputs
    {
        float speed = 0.0f;
        float steering = 0.0f;
    };

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_ref_speed(float const speed)
    {
        ref.speed = speed;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    float get_ref_speed()
    {
        return ref.speed;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_ref_steering(float const steering)
    {
        ref.steering = steering;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    float get_ref_steering()
    {
        return ref.steering;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    float get_steering()
    {
        return outputs.steering;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    float get_speed()
    {
        return outputs.speed;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_ref_acceleration(float const acc, float const dt)
    {
        ref.speed += acc * dt;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void set_ref_steering_rate(float const steering, float const dt)
    {
        ref.steering += steering * dt;
    }

    //----------------------------------------------------------------------
    //! \brief
    //----------------------------------------------------------------------
    void update(float const /*dt*/ /*, sensors*/)
    {
        //
        outputs.speed = ref.speed;
        outputs.steering = ref.steering;
    }

    References ref;
    Inputs inputs;
    Outputs outputs;
};

#endif
