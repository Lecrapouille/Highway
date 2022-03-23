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

#include "Vehicle/VehiclePhysicalModels/TricycleKinematic.hpp"
#include <iostream>

//------------------------------------------------------------------------------
void TrailerKinematic::init(float acceleration, float speed, sf::Vector2f position,
                            float heading)
{
    float x = 0.0f;
    float y = 0.0f;

    m_speed = speed;

    // See "Flatness and motion Planning: the Car with n-trailers" by Pierre Rouchon ...
    // x0 - sum^{n}_{i=1}(cos(heading_i) d_i)
    // y0 - sum^{n}_{i=1}(sin(heading_i) d_i)
    // i: the nth trailer and (x0, y0) middle of the rear axle of the car
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
           float d = f->m_shape.dim.wheelbase;

           std::cout << "Trailer: '" << front->name << "': " << RAD2DEG(f->heading()) << " " << d << std::endl;
           x = x + cosf(f->heading()) * d;
           y = y + sinf(f->heading()) * d;
           std::cout << "=> " << x << ", " << y << std::endl;
       }
       front = front->next;
    }
}
