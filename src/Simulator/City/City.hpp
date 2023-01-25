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

    City(/*TrafficSide const side*/);

    //-------------------------------------------------------------------------
    //! \brief Reset the simulation states, remove entities: parking, cars, ego
    //! car ...
    //-------------------------------------------------------------------------
    void reset();

    Road& addRoad(std::vector<sf::Vector2<Meter>> const& centers,
                  Meter const width, std::array<size_t, TrafficSide::Max> lanes);

    //-------------------------------------------------------------------------
    //! \brief Add a parking slot in the world at the given position. The
    //! parking instance is hold by the simulation instance.
    //! \param[in] type: non NULL string of the mark of the vehicle for its
    //! dimension.
    //! \param[in] position: the position of the middle of the left lane inside
    //! the world coordinates.
    //! \return the reference of the created parking.
    //-------------------------------------------------------------------------
    Parking& addParking(const char* type, sf::Vector2<Meter> const& position,
                        Radian const heading);

    //-------------------------------------------------------------------------
    //! \brief Add a new parking slot next to the given one. The
    //! parking instance is hold by the simulation instance.
    //! \param[in] parking: previous parking slot.
    //! \return the reference of the created parking.
    //-------------------------------------------------------------------------
    Parking& addParking(Parking const& parking);

    //-------------------------------------------------------------------------
    //! \brief Add a parking slot along a road. The parking instance is hold by
    //! the simulation instance.
    //! \param[in] type: non NULL string of the mark of the vehicle for its
    //! dimension.
    //! \param[in] road: the road to place the vehicle on.
    //! \param[in] side: the traffic side to place the vehicle on.
    //! \param[in] offset_long: the percentage of placement of the vehicle along
    //! the logitudinal axis of the lane. 0.0: place the vehicle at the initial
    //! position of the lane. 0.5: place in the middle of the road. 1.0: place
    //! at the end of the lane.
    //! \param[in] offset_lat: the percentage of placement of the vehicle along
    //! the lateral axis of the lane. 0.0: place the vehicle at the right of the
    //! lane. 0.5: place in the middle of the road. 1.0: place at the left of the
    //! lane.
    //! \param[in] speed: initial longitudinal speed (m/s). By default: 0 m/s.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    Parking& addParking(const char* type, Road const& road, TrafficSide const side,
                        double const offset_long, double const offset_lat = 0.0f);

    //-------------------------------------------------------------------------
    //! \brief Create or replace the ego vehicle (the autonomous vehicle). The
    //! instance is hold by the simulation instance.
    //! \note: Only one ego vehicle is managed for the moment.
    //! \param[in] model: non NULL string of the mark of the vehicle for its
    //! dimension.
    //! \param[in] position: the position of the middle of the rear axle inside
    //! the world coordinates.
    //! \param[in] heading: the vehicle direction (yaw angle) in degree.
    //! \param[in] speed: initial longitudinal speed (m/s). By default: 0 m/s.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    Car& addEgo(const char* model, sf::Vector2<Meter> const& position,
                Degree const heading = 0.0_deg, MeterPerSecond const speed = 0.0_mps);

    //-------------------------------------------------------------------------
    //! \brief Create or replace the ego vehicle (the autonomous vehicle) on a
    //! road. The instance is hold by the simulation instance.
    //! \fixme check against collision.
    //! \note: Only one ego vehicle is managed for the moment.
    //! \param[in] model: non NULL string of the mark of the vehicle for its
    //! dimension.
    //! \param[in] road: the road to place the vehicle on.
    //! \param[in] side: the traffic side to place the vehicle on.
    //! \param[in] lane: the lane to place the vehicle on.
    //! \param[in] offset_long: the percentage of placement of the vehicle along
    //! the logitudinal axis of the lane. 0.0: place the vehicle at the initial
    //! position of the lane. 0.5: place in the middle of the road. 1.0: place
    //! at the end of the lane.
    //! \param[in] offset_lat: the percentage of placement of the vehicle along
    //! the lateral axis of the lane. 0.0: place the vehicle at the right of the
    //! lane. 0.5: place in the middle of the road. 1.0: place at the left of the
    //! lane.
    //! \param[in] speed: initial longitudinal speed (m/s). By default: 0 m/s.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    Car& addEgo(const char* model, Road const& road, TrafficSide const side,
                size_t const lane, double const offset_long, double const offset_lat,
                MeterPerSecond const speed = 0.0_mps);

    //-------------------------------------------------------------------------
    //! \brief Create a new car. The instance is hold by the simulation instance.
    //! \note: Only one ego vehicle is managed for the moment.
    //! \param[in] model: non NULL string of the mark of the vehicle for its
    //! dimension.
    //! \param[in] position: the position of the middle of the rear axle inside
    //! the world coordinates.
    //! \param[in] heading: the vehicle direction (yaw angle) in degree.
    //! \param[in] speed: initial longitudinal speed (m/s). By default: 0 m/s.
    //! \param[in] steering: initial steering angle (in degree). By default: 0
    //! rad.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    Car& addCar(const char* model, sf::Vector2<Meter> const& position, Degree const heading,
                MeterPerSecond const speed, Degree const steering = 0.0_deg);

    //-------------------------------------------------------------------------
    //! \brief Add a new car and place it on a road. The instance is hold by
    //! the simulation instance.
    //! \fixme check against collision.
    //! \note: Only one ego vehicle is managed for the moment.
    //! \param[in] model: non NULL string of the mark of the vehicle for its
    //! dimension.
    //! \param[in] road: the road to place the vehicle on.
    //! \param[in] side: the traffic side to place the vehicle on.
    //! \param[in] lane: the lane to place the vehicle on.
    //! \param[in] offset_long: the percentage of placement of the vehicle along
    //! the logitudinal axis of the lane. 0.0: place the vehicle at the initial
    //! position of the lane. 0.5: place in the middle of the road. 1.0: place
    //! at the end of the lane.
    //! \param[in] offset_lat: the percentage of placement of the vehicle along
    //! the lateral axis of the lane. 0.0: place the vehicle at the right of the
    //! lane. 0.5: place in the middle of the road. 1.0: place at the left of the
    //! lane.
    //! \param[in] speed: initial longitudinal speed (m/s). By default: 0 m/s.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    Car& addCar(const char* model, Road const& road, TrafficSide const side,
                size_t const lane, double const offset_long, double const offset_lat,
                MeterPerSecond const speed = 0.0_mps);

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
    //! \param[in] heading: the vehicle direction (yaw angle) in degree.
    //! \param[in] steering: initial steering angle (in degree). By default: 0
    //! rad.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    Car& addGhost(const char* model, sf::Vector2<Meter> const& position, Degree const heading,
                  Degree const steering); // FIXME stored in m_cars or m_ghosts or move it simulator

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
    //! \brief Return the list of parkings.
    //-------------------------------------------------------------------------
    std::vector<std::unique_ptr<Road>>& roads()
    {
        return m_roads;
    }

    //-------------------------------------------------------------------------
    //! \brief Return the altitude at the given coordinates. TODO
    //-------------------------------------------------------------------------
    Meter altitude(sf::Vector2f const& position)
    {
        return 0.0_m; // Not implemented yet
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
    //! \param[in] heading: the vehicle direction (yaw angle) in degree.
    //! \param[in] speed: initial longitudinal speed (m/s). By default: 0 m/s.
    //! \param[in] steering: initial steering angle (in degree). By default: 0
    //! rad.
    //! \return the reference of the created vehicle.
    //-------------------------------------------------------------------------
    template<class CAR>
    std::unique_ptr<CAR> createCar(const char* model, const char* name, sf::Color color,
                                   MeterPerSecondSquared const acceleration, MeterPerSecond const speed,
                                   sf::Vector2<Meter> const& position, Degree const heading,
                                   Degree const steering)
    {
        std::unique_ptr<CAR> car = std::make_unique<CAR>(model, color);
        car->name = name;
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
    std::vector<std::unique_ptr<Road>> m_roads; // FIXME: graph<Road, Carrefour> or Lanes https://github.com/Lecrapouille/Highway/issues/24
    //! \brief Container of parking slots
    std::vector<std::unique_ptr<Parking>> m_parkings; // FIXME non pointers
    // TODO roads and bounding boxes of static objects, pedestrians
    // TODO grid to detect collisions and detect objects around

private:

    size_t m_car_id = 0u;
    size_t m_ego_id = 0u;
    size_t m_ghost_id = 0u;
};

#endif
