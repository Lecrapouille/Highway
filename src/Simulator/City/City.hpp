//=====================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2022 Quentin Quadrat <lecrapouille@gmail.com>
//
// This file is part of Highway.
//
// Highway is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#ifndef CITY_HPP
#  define CITY_HPP

// #  include "City/Drivers.hpp" FIXME TBD
#  include "Common/SpatialHashGrid.hpp"
#  include "City/Parking.hpp"
#  include "City/Road.hpp"
#  include "City/Pedestrian.hpp"
#  include "Vehicle/Vehicles.hpp"
#  include <vector>
#  include <memory>

// TODO:https://www.mathworks.com/help/driving/ug/create-driving-scenario-interactively-and-generate-synthetic-detections.html
// TODO show the grid
// TODO Binary space partition to faster collisions.
// TODO search in bsp for getting actors around the ego vehicle.

// ****************************************************************************
//! \brief Class managing a collection of static actors (roads, parkings), and
//! dynamic actors (vehicles, pedestrians ...). The city shall be seen as a
//! container (with binary space partitioning (in gestation) to faster
//! collision and searching objects) and is passive: it does not perform simulation which is made by
//! Simulator class.
// ****************************************************************************
class City
{
public:

    City();

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
    Car& addEgo(const char* model, sf::Vector2f const& position,
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
    //! \brief Find and return the address of the desired car from its name.
    //! return nullptr in case if not found.
    //-------------------------------------------------------------------------
    Car* get(const char* name);

    //-------------------------------------------------------------------------
    //! \brief Return the list of vehicles.
    //-------------------------------------------------------------------------
    std::vector<std::unique_ptr<Car>>& cars()
    {
        return m_cars;
    }

    std::vector<std::unique_ptr<Car>> const& cars() const
    {
        return m_cars;
    }

    std::vector<std::unique_ptr<Car>> const& ghosts() const
    {
        return m_ghosts;
    }

    std::unique_ptr<Car> const& ego() const
    {
        return m_ego;
    }

    //-------------------------------------------------------------------------
    //! \brief Return the list of parkings.
    //-------------------------------------------------------------------------
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

    //-------------------------------------------------------------------------
    //! \brief Return ref const to the hash grid.
    //-------------------------------------------------------------------------
    //inline SpatialHashGrid const& grid() const
    //{
    //    return m_grid;
    //}

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
    std::unique_ptr<CAR> createCar(const char* model, const char* name, sf::Color color,
                                   float const acceleration, float const speed,
                                   sf::Vector2f const& position, float const heading,
                                   float const steering)
    {
        std::unique_ptr<CAR> car = std::make_unique<CAR>(model, color);
        car->name = name;
        car->init(acceleration, speed, position, heading, steering);
        return car;
    }

protected:

    //! \brief
    //SpatialHashGrid m_grid;
    //! \brief Container of cars
    std::vector<std::unique_ptr<Car>> m_cars;
    //! \brief Container of purely displayed cars
    std::vector<std::unique_ptr<Car>> m_ghosts;
    //! \brief The autonomous cars (TODO for the moment only one is managed)
    std::unique_ptr<Car> m_ego = nullptr;
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
