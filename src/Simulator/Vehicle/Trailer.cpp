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

#  include "Vehicle/Trailer.hpp"
//FIXME#  include "Vehicle/VehiclePhysicalModels/TrailerKinematic.hpp"

//------------------------------------------------------------------------------
Trailer::Trailer(const char* name_, sf::Color const& color_)
    : Vehicle<TrailerBluePrint>(BluePrints::get<TrailerBluePrint>(name_), name_, color_)
{
    std::cout << "Trailer " << name_ << std::endl;
    //m_physics = std::make_unique<TrailerKinematic>(*m_shape, *m_control);
}

//------------------------------------------------------------------------------
void Trailer::update_wheels(float const speed, float const steering)
{
    m_wheels[TrailerBluePrint::WheelName::RL].speed = speed;
    m_wheels[TrailerBluePrint::WheelName::RR].speed = speed;

    m_wheels[TrailerBluePrint::WheelName::RL].steering = 0.0f;
    m_wheels[TrailerBluePrint::WheelName::RR].steering = 0.0f;
}
