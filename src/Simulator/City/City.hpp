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
#  include "Vehicle/SelfParkingCar.hpp"

#  include <deque>

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
    //! \brief Create a predefined parking world (work in progress).
    //! \param[in] angle: the type of parking slots. Accepted values:
    //! - 0: parallel slots,
    //! - 90: perpendicular slots,
    //! - 45, 60, 75: diagonal slots.
    //! \param[in] parked: set to true to force the ego to be parked inside a
    //! parking spot.
    //-------------------------------------------------------------------------
    void createWorld(size_t const angle, bool const parked);

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
    SelfParkingCar& addEgo(const char* model, sf::Vector2f const& position,
                           float const heading, float const speed = 0.0f,
                           float const steering = 0.0f);

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
    //! \brief Create a vehicle. The instance is hold by the simulation instance.
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
                float const speed = 0.0f, float const steering = 0.0f);

    //-------------------------------------------------------------------------
    //! \brief Create a ghost parked car. The car instance is hold by the
    //! simulation instance. Ghost cars do not intreact with the world, other
    //! cars. Their goal is purely for display and screenshots.
    //! \param[in] model: non NULL string of the mark of the vehicle for its
    //! dimension.
    //! \param[in] parking: the reference of the parking slot in which the car
    //! shall be parked in.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    Car& addGhost(const char* model, Parking& parking);

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    Car& addGhost(const char* model, sf::Vector2f const& position,
                  float const heading, float const steering = 0.0f);

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
    Parking& addParking(ParkingBluePrint const& dim, sf::Vector2f const& position);

    //-------------------------------------------------------------------------
    //! \brief Return the altitude at the given coordinates. TODO
    //-------------------------------------------------------------------------
    float altitude(sf::Vector2f const& position)
    {
        return 0.0f; // Not implemented yet
    }

    //FIXME protected:

    //! \brief Container of parked cars
    std::deque<std::unique_ptr<Car>> m_cars;
    //! \brief Container of purely displayed cars
    std::deque<std::unique_ptr<Car>> m_ghosts;
    //! \brief The autonomous cars (TODO for the moment only one is managed)
    std::unique_ptr<SelfParkingCar> m_ego = nullptr;
    //! \brief Container of parking slots
    std::deque<Parking> m_parkings;
    // TODO roads and bounding boxes of static objects, pedestrians
    // TODO grid to detect collisions and detect objects around
};

#endif
