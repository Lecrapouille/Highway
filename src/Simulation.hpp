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

#ifndef SIMULATION_HPP
#  define SIMULATION_HPP

#  include "SelfParking/SelfParkingVehicle.hpp"
#  include "World/Parking.hpp"

// ****************************************************************************
//! \brief
// ****************************************************************************
class Simulation
{
    friend class GUISimulation;

public:

    //-------------------------------------------------------------------------
    //! \brief Draw the world and its entities.
    //-------------------------------------------------------------------------
    void draw(sf::RenderWindow& renderer, sf::View& view);

    //-------------------------------------------------------------------------
    //! \brief Update the simuation states.
    //-------------------------------------------------------------------------
    void update(const float dt);

    //-------------------------------------------------------------------------
    //! \brief Reset entities: parking, cars, ego car ...
    //-------------------------------------------------------------------------
    void clear();

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    void createWorld(size_t angle, bool const entering);

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    SelfParkingCar& addEgo(const char* model, sf::Vector2f const& position, float const heading,
                  float const speed = 0.0f, float const steering = 0.0f);

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    SelfParkingCar& addEgo(CarDimension const& dim, sf::Vector2f const& position, float const heading,
                  float const speed = 0.0f, float const steering = 0.0f);

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    Car& addCar(const char* model, Parking& parking);

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    Car& addCar(const char* model, sf::Vector2f const& position, float const heading,
                float const speed = 0.0f, float const steering = 0.0f);

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    Car& addCar(CarDimension const& dim, sf::Vector2f const& position, float const heading,
                float const speed, float const steering);

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    Parking& addParking(const char* type, sf::Vector2f const& position);

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    Parking& addParking(ParkingDimension const& dim, sf::Vector2f const& position);

    //-------------------------------------------------------------------------
    //! \brief Return the altitude at the given coordinates.
    //-------------------------------------------------------------------------
    float altitude(sf::Vector2f const& position)
    {
        return 0.0f; // Not implemented yet
    }

protected:

    //! \brief Container of parked cars
    std::deque<std::unique_ptr<Car>> m_cars;
    //! \brief The autonomous cars
    std::unique_ptr<SelfParkingCar> m_ego = nullptr;
    //! \brief Container of parking slots
    std::deque<Parking> m_parkings;
    // TODO roads and bounding boxes of objects
};

#endif
