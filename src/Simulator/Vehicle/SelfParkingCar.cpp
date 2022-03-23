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

#  include "Vehicle/SelfParkingCar.hpp"

//------------------------------------------------------------------------------
SelfParkingCar::SelfParkingCar(const char* name_, sf::Color const& color_)
    : Car(name_, color_), m_auto_park(addComponent<SelfParkingComponent>())
{
    addRadar({ .offset = sf::Vector2f(blueprint.wheelbase + blueprint.front_overhang, 0.0f), 
               .orientation = 90.0f,
               .fov = 20.0f,
               .range = 2.0f/*174.0f*/ });

    // Make the car react to some I/O events
    registerCallback(sf::Keyboard::PageDown, [&]() {
        turningIndicator(false, m_turning_right ^ true);
    });
    registerCallback(sf::Keyboard::PageUp, [&]() {
        turningIndicator(m_turning_left ^ true, false);
    });
    registerCallback(sf::Keyboard::Up, [&]() {
        refSpeed(1.0f);
    });
    registerCallback(sf::Keyboard::Down, [&]() {
        refSpeed(0.0f);
    });
    registerCallback(sf::Keyboard::Right, [&]() {
        refSteering(refSteering() - 0.1f);
    });
    registerCallback(sf::Keyboard::Left, [&]() {
        refSteering(refSteering() + 0.1f);
    });
}

//------------------------------------------------------------------------------
void SelfParkingCar::update(float const dt)
{
   m_auto_park.update();
   Car::update(dt);
}
