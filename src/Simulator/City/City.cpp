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

#include "City/City.hpp"
#include <iostream>

#define COLISION_COLOR sf::Color(255, 0, 0)
#define CAR_COLOR sf::Color(178, 174, 174)
#define EGO_CAR_COLOR sf::Color(124, 99, 197)

//------------------------------------------------------------------------------
void City::reset()
{
    m_car_id = m_ego_id = m_ghost_id = 0u;

    //m_ghosts.clear();
    m_cars.clear();
    m_parkings.clear();
}

//------------------------------------------------------------------------------
Car* City::get(const char* name)
{
   for (auto& it: m_cars)
   {
      if (it->name == name)
         return &(*it);
   }

   return nullptr;
}

//------------------------------------------------------------------------------
Parking& City::addParking(const char* type, sf::Vector2f const& position)
{
    m_parkings.push_back(std::make_unique<Parking>(BluePrints::get<ParkingBluePrint>(type), position));
    return *m_parkings.back();
}

//------------------------------------------------------------------------------
SelfParkingCar& City::addEgo(const char* model, sf::Vector2f const& position,
                             float const heading, float const speed)
{
    std::string name = "ego" + std::to_string(m_ego_id++);

    return createCar<SelfParkingCar>(model, name.c_str(), EGO_CAR_COLOR, 0.0f, speed,
        position, heading, 0.0f);
}

//------------------------------------------------------------------------------
Car& City::addCar(const char* model, sf::Vector2f const& position, float const heading,
                  float const speed, float const steering)
{
    std::string name = "car" + std::to_string(m_car_id++);

    return createCar<Car>(model, name.c_str(), CAR_COLOR, 0.0f, speed, position,
       heading, steering);
}

//------------------------------------------------------------------------------
Car& City::addCar(const char* model, Parking& parking)
{
    Car& car = addCar(model, sf::Vector2f(0.0f, 0.0f), 0.0f, 0.0f);
    parking.bind(car);
    return car;
}

//------------------------------------------------------------------------------
Car& City::addGhost(const char* model, sf::Vector2f const& position, float const heading,
                    float const steering)
{
    std::string name = "ghost" + std::to_string(m_ghost_id++);

    return createCar<Car>(model, name.c_str(), sf::Color::White, 0.0f, 0.0f, position,
       heading, steering);
}
