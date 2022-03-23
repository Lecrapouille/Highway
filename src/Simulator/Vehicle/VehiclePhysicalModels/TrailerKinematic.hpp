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

#ifndef TRAILER_KINEMATIC_HPP
#  define TRAILER_KINEMATIC_HPP

#include "Vehicle/VehiclePhysics.hpp"

// *****************************************************************************
//! \brief Class computing the kinematic equation of a trailer.
//! A trailer can be attached to another trailer or to a tractor vehicle.
// *****************************************************************************
class TrailerKinematic: public VehiclePhysics
{
public:

    //--------------------------------------------------------------------------
    virtual void init(float acceleration, float speed, sf::Vector2f position,
                      float heading) override;

    //--------------------------------------------------------------------------
    //! \brief Concrete method updating vehicle physics equations.
    //! See "Flatness and motion Planning: the Car with n-trailers" by Pierre
    //! Rouchon ...
    //! \param[in] control: the cruise control of the head vehicle.
    //! \param[in] dt: delta time [seconds] from the previous call.
    //--------------------------------------------------------------------------
    template<class BLUEPRINT>
    virtual void update(VehicleShape<BLUEPRINT> const& control, VehicleControl
                        const& control, float const dt) override
{
    std::cout << "Trailer update " << name << std::endl;
    assert(next != nullptr);

    m_speed = control.outputs.body_speed;
    float heading = m_shape.heading();
    float x = 0.0f;
    float y = 0.0f;

    if (next->next == nullptr)
    {
        heading += dt * m_speed * sinf(next->heading() - heading) / m_shape.dim.wheelbase;
    }
    else
    {
        std::cerr << "not yet managed" << std::endl;
        exit(1);
    }

    IPhysics* front = this;
    while (front != nullptr)
    {
       if (front->next == nullptr)
       {
           x = front->position().x - x;
           y = front->position().y - y;
           std::cout << "car: '" << front->name << "': " << front->position().x << ", " << front->position().y << std::endl;
           std::cout << "=> " << x << ", " << y << std::endl;
       }
       else
       {
           TrailerKinematic* f = reinterpret_cast<TrailerKinematic*>(front);
           float heading = f->heading();
           float d = f->m_shape.dim.wheelbase;

           std::cout << "Trailer: '" << front->name << "': " << RAD2DEG(heading) << " " << d << std::endl;
           x = x + cosf(heading) * d;
           y = y + sinf(heading) * d;
           std::cout << "=> " << x << ", " << y << std::endl;
       }
       front = front->next;
    }
}

private:

};

#endif
