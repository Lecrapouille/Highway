//=====================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2023 Quentin Quadrat <lecrapouille@gmail.com>
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
// along with Highway.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#pragma once

#  include "Core/Simulator/Vehicle/Car.hpp"
#  include "Core/Math/Units.hpp"

#  include <SFML/Graphics/Color.hpp>

#  include <vector>
#  include <memory>

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

    City(/*TrafficSide const side*/);

    //-------------------------------------------------------------------------
    //! \brief Reset the simulation states, remove entities: parking, cars, ego
    //! car ...
    //-------------------------------------------------------------------------
    void reset();

    //-------------------------------------------------------------------------
    //! \brief Create a new car. The instance is hold by the simulation instance.
    //! \note: Only one ego vehicle is managed for the moment.
    //! \param[in] model: non NULL string of the mark of the vehicle for its
    //! dimension.
    //! \param[in] position: the position of the middle of the rear axle inside
    //! the world coordinates.
    //! \param[in] heading: the vehicle direction (yaw angle) in rad.
    //! \param[in] speed: initial longitudinal speed (m/s). By default: 0 m/s.
    //! \param[in] steering: initial steering angle (in rad). By default: 0
    //! rad.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    Car& addCar(const char* model, sf::Vector2<Meter> const& position, Radian const heading,
                MeterPerSecond const speed, Radian const steering = 0.0_deg);

    Car& addEgo(const char* model, sf::Vector2<Meter> const& position, Radian const heading,
                MeterPerSecond const speed, Radian const steering = 0.0_deg);

    //-------------------------------------------------------------------------
    //! \brief Return the list of vehicles.
    //-------------------------------------------------------------------------
    std::vector<std::unique_ptr<Car>> const& cars() const
    {
        return m_cars;
    }

    Car const& ego() const
    {
        assert(m_ego != nullptr && "The simulation needs a single ego car");
        return *m_ego;
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
    //! \param[in] heading: the vehicle direction (yaw angle) in rad.
    //! \param[in] speed: initial longitudinal speed (m/s). By default: 0 m/s.
    //! \param[in] steering: initial steering angle (in rad). By default: 0
    //! rad.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    template<class PhysicModel>
    std::unique_ptr<Car> createCar(const char* model, const char* name, sf::Color color,
                                   MeterPerSecondSquared const acceleration, MeterPerSecond const speed,
                                   sf::Vector2<Meter> const& position, Radian const heading,
                                   Radian const steering)
    {
        std::cout << "City createCar " << this << std::endl;
        std::unique_ptr<Car> car = std::make_unique<Car>(model, name, color);
        car->setPhysicModel<PhysicModel>(acceleration, speed, position, heading);
        car->init(acceleration, speed, position, heading, steering);
        return car;
    }

protected:

    //! \brief
    //SpatialHashGrid m_grid; FIXME https://github.com/Lecrapouille/Highway/issues/23
    //! \brief Container of cars
    std::vector<std::unique_ptr<Car>> m_cars; // FIXME weak_ptr
    //! \brief Container of purely displayed cars
    std::vector<std::unique_ptr<Car>> m_ghosts;
    //! \brief The autonomous cars (TODO for the moment only one is managed)
    std::unique_ptr<Car> m_ego = nullptr;
    //! \brief Container of roads
    //std::vector<std::unique_ptr<Road>> m_roads; // FIXME: graph<Road, Carrefour> or Lanes https://github.com/Lecrapouille/Highway/issues/24
    //! \brief Container of parking slots
    //std::vector<std::unique_ptr<Parking>> m_parkings; // FIXME non pointers
    // TODO roads and bounding boxes of static objects, pedestrians
    // TODO grid to detect collisions and detect objects around

private:

    size_t m_car_id = 0u;
    size_t m_ego_id = 0u;
    size_t m_ghost_id = 0u;
};