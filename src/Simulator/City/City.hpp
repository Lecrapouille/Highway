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

#ifndef CITY_HPP
#  define CITY_HPP

#  include "City/Parking.hpp"
#  include "City/Road.hpp"
#  include "City/Pedestrian.hpp"
// #  include "City/Drivers.hpp" FIXME TBD
#  include "Simulation/SelfParkingCar.hpp"

#  include <vector>

// TODO:https://www.mathworks.com/help/driving/ug/create-driving-scenario-interactively-and-generate-synthetic-detections.html

// ****************************************************************************
//! \brief Class managing car simulation.
// ****************************************************************************
class City
{
public:

    //-------------------------------------------------------------------------
    //! \brief Reset the simulation states, remove entities: parking, cars, ego
    //! car ...
    //-------------------------------------------------------------------------
    void reset();

    //-------------------------------------------------------------------------
    //! \brief Add a parking slot in the world at the given position. The
    //! parking instance is hold by the simulation instance.
    //! \param[in] type: non NULL string of the mark of the vehicle for its
    //! dimension.
    //! \param[in] position: the position of the middle of the left lane inside
    //! the world coordinates.
    //! \return the reference of the created parking.
    //-------------------------------------------------------------------------
    Parking& addParking(const char* type, sf::Vector2f const& position);

    //-------------------------------------------------------------------------
    //! \brief Create or replace the ego vehicle (the autonomous vehicle). The
    //! instance is hold by the simulation instance.
    //! \note: Only one ego vehicle is managed for the moment.
    //! \param[in] model: non NULL string of the mark of the vehicle for its
    //! dimension.
    //! \param[in] position: the position of the middle of the rear axle inside
    //! the world coordinates.
    //! \param[in] heading: the vehicle direction (yaw angle) in radian.
    //! \param[in] speed: initial longitudinal speed (m/s). By default: 0 m/s.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    SelfParkingCar& addEgo(const char* model, sf::Vector2f const& position,
                           float const heading = 0.0f, float const speed = 0.0f);

    //-------------------------------------------------------------------------
    //! \brief Create or replace the ego vehicle (the autonomous vehicle). The
    //! instance is hold by the simulation instance.
    //! \note: Only one ego vehicle is managed for the moment.
    //! \param[in] model: non NULL string of the mark of the vehicle for its
    //! dimension.
    //! \param[in] position: the position of the middle of the rear axle inside
    //! the world coordinates.
    //! \param[in] heading: the vehicle direction (yaw angle) in radian.
    //! \param[in] speed: initial longitudinal speed (m/s). By default: 0 m/s.
    //! \param[in] steering: initial steering angle (in radian) ). By default: 0
    //! rad.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    Car& addCar(const char* model, sf::Vector2f const& position, float const heading,
                float const speed, float const steering = 0.0f);

    //-------------------------------------------------------------------------
    //! \brief Create a parked car. The car instance is hold by the simulation
    //! instance.
    //! \param[in] model: non NULL string of the mark of the vehicle for its
    //! dimension.
    //! \param[in] parking: the reference of the parking slot in which the car
    //! shall be parked in.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    Car& addCar(const char* model, Parking& parking);

    //-------------------------------------------------------------------------
    //! \brief Create or replace a static vehicle not interacting with the city.
    //! Ghost are only used for debugging by rendering a vehicle. The
    //! instance is hold by the simulation instance.
    //! \note: Only one ego vehicle is managed for the moment.
    //! \param[in] model: non NULL string of the mark of the vehicle for its
    //! dimension.
    //! \param[in] position: the position of the middle of the rear axle inside
    //! the world coordinates.
    //! \param[in] heading: the vehicle direction (yaw angle) in radian.
    //! \param[in] steering: initial steering angle (in radian) ). By default: 0
    //! rad.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    Car& addGhost(const char* model, sf::Vector2f const& position, float const heading,
                  float const steering); // FIXME stored in m_cars or m_ghosts or move it simulator

    //-------------------------------------------------------------------------
    //! \brief Return the car by its name.
    //-------------------------------------------------------------------------
    Car* get(const char* name);

    //-------------------------------------------------------------------------
    //! \brief Return
    //-------------------------------------------------------------------------
    std::vector<std::unique_ptr<Car>>& cars()
    {
        return m_cars;
    }

    std::vector<std::unique_ptr<Parking>>& parkings()
    {
        return m_parkings;
    }

    //-------------------------------------------------------------------------
    //! \brief Return the altitude at the given coordinates. TODO
    //-------------------------------------------------------------------------
    float altitude(sf::Vector2f const& position)
    {
        return 0.0f; // Not implemented yet
    }

protected:

    //-------------------------------------------------------------------------
    //! \brief Create or replace the ego vehicle (the autonomous vehicle). The
    //! instance is hold by the simulation instance.
    //! \note: Only one ego vehicle is managed for the moment.
    //! \param[in] model: non NULL string of the mark of the vehicle for its
    //! dimension.
    //! \param[in] position: the position of the middle of the rear axle inside
    //! the world coordinates.
    //! \param[in] heading: the vehicle direction (yaw angle) in radian.
    //! \param[in] speed: initial longitudinal speed (m/s). By default: 0 m/s.
    //! \param[in] steering: initial steering angle (in radian) ). By default: 0
    //! rad.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    template<class CAR>
    CAR& createCar(const char* model, const char* name, sf::Color color, float const acceleration,
                   float const speed, sf::Vector2f const& position,
                   float const heading, float const steering)
    {
        std::unique_ptr<CAR> car = std::make_unique<CAR>(model, color);
        car->name = name;
        car->init(acceleration, speed, position, heading, steering);
        m_cars.push_back(std::move(car));

        return static_cast<CAR&>(*m_cars.back());
    }

protected:

    //! \brief Container of cars
    std::vector<std::unique_ptr<Car>> m_cars;
    //! \brief Container of parking slots
    std::vector<std::unique_ptr<Parking>> m_parkings;
    // TODO roads and bounding boxes of static objects, pedestrians
    // TODO grid to detect collisions and detect objects around

private:

    size_t m_car_id = 0u;
    size_t m_ego_id = 0u;
    size_t m_ghost_id = 0u;
};

#endif
