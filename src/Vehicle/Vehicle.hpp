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

#ifndef VEHICLE_HPP
#  define VEHICLE_HPP

#  include "World/Blueprints.hpp"
#  include "World/Parking.hpp"
#  include "Vehicle/VehiclePhysics.hpp"
#  include <memory>
#  include <deque>
#  include <atomic>

//! \brief Default sf::Color for a car
#define DEFAULT_VEHICLE_COLOR 178, 174, 174

// *****************************************************************************
//! \brief Class defining a trailer with its dimension, physics.
// *****************************************************************************
class Trailer
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor: trailer with its dimension. The attitude is
    //! set with the init() method.
    //! \param[in] dimension: dimension structure returned by
    //! TrailerDimensions::get().
    //! \param[in] front: the front vehicle (car or trailer).
    //--------------------------------------------------------------------------
    Trailer(TrailerDimension const& dimension, IPhysics& front)
        : dim(dimension), m_shape(dim), m_kinematic(name, m_shape, front)
    {}

    //--------------------------------------------------------------------------
    //! \brief Initialize first value for the physics.
    //! \param[in] speed: initial longitudinal speed [m/s] (usually 0).
    //! \param[in] heading: initial yaw of the car [rad]
    //! \note: the position is computed from the front vehicle.
    //--------------------------------------------------------------------------
    inline void init(float const speed, float const heading)
    {
        m_kinematic.init(speed, heading);
    }

    //--------------------------------------------------------------------------
    //! \brief Update the physics of the trailer.
    //--------------------------------------------------------------------------
    void update(CarControl control, float const dt)
    {
        m_kinematic.update(control, dt);
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return longitudinal acceleration [meter/seconds^2].
    //--------------------------------------------------------------------------
    inline float acceleration() const
    {
        return m_kinematic.acceleration();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return longitudinal speed [meter/seconds].
    //--------------------------------------------------------------------------
    inline float speed() const
    {
        return m_kinematic.speed();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return position of the middle of the rear axle.
    //--------------------------------------------------------------------------
    inline sf::Vector2f position() const
    {
        return m_kinematic.position();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading [rad].
    //--------------------------------------------------------------------------
    inline float heading() const
    {
        return m_shape.heading();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the trailer shape.
    //--------------------------------------------------------------------------
    inline TrailerShape const& shape() const
    {
        return m_shape;
    }

    //--------------------------------------------------------------------------
    //! \brief Non const getter: return the kinematic.
    //--------------------------------------------------------------------------
    inline TrailerKinematic& kinematic()
    {
        return m_kinematic;
    }

    //-------------------------------------------------------------------------
    //! \brief Debug purpose only: show shape information.
    //-------------------------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, Trailer const& trailer)
    {
        return os << "Trailer " << trailer.name << " {" << std::endl
                  //TODO << trailer.dim << std::endl
                  << trailer.m_shape << std::endl
                  << "}";
    }

public:

    //! \brief Trailer's read-only dimension.
    TrailerDimension const dim;

    //! \brief Car's name
    std::string name = "trailer";

    //! \brief Current car color. Public: to allow to change it for distinguish
    //! car between them or for showing collisions ...
    sf::Color color = sf::Color(DEFAULT_VEHICLE_COLOR);

private:

    //! \brief Trailer blueprint.
    TrailerShape m_shape;
    //! \brief Trailer Physics.
    TrailerKinematic m_kinematic;
};

// ****************************************************************************
//! \brief Class defining a car with its dimension, physics, controler and
//! knowin how to park by itself.
// ****************************************************************************
class Car
{
public:

    //--------------------------------------------------------------------------
    //! \brief Default constructor: car with its dimension. The attitude is set
    //! with the init() method.
    //! \param[in] dimension: dimension structure returned by CarDimensions::get().
    //--------------------------------------------------------------------------
    Car(CarDimension const& dimension)
        : dim(dimension), m_shape(dim), m_kinematic(name, m_shape)
    {}

    //-------------------------------------------------------------------------
    //! \brief Default constructor: car with its dimension deduced by the model.
    //! The car attitude is set with the init() method.
    //! \param[in] model of the car. Shall be known from CarDimensions and shall
    //! not be NULL.
    //-------------------------------------------------------------------------
    Car(const char* model)
        : Car(CarDimensions::get(model))
    {}

    //-------------------------------------------------------------------------
    //! \brief Needed because of virtual methods.
    //-------------------------------------------------------------------------
    virtual ~Car() = default;

    //-------------------------------------------------------------------------
    //! \brief Initialize first value for the physics.
    //! \param[in] position: position of the middle of the rear axle.
    //! \param[in] heading: initial yaw of the car [rad]
    //! \param[in] speed: initial longitudinal speed [m/s] (usually 0).
    //! \param[in] steering: initial front wheels orientation [rad] (usually 0).
    //-------------------------------------------------------------------------
    virtual void init(sf::Vector2f const& position, float const heading,
                      float const speed = 0.0f, float const steering = 0.0f)
    {
        m_kinematic.init(position, heading, speed, steering);

        for (auto& it: m_trailers)
        {
            it->init(speed, 0.0f); // TODO memorize headings
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Attach a trailer at the end of the link of trailers. The position
    //! of the trailer is deduced.
    //! \param[in] dimension: the dimension of the trailer.
    //! \param[in] heading: the angle between the trailer and the front vehicle.
    //-------------------------------------------------------------------------
    Trailer& attachTrailer(TrailerDimension const& dimension, const float heading)
    {
        IPhysics* phys;

        if (m_trailers.empty())
        {
            phys = &m_kinematic;
        }
        else
        {
            phys = &(m_trailers.back()->kinematic());
        }

        m_trailers.push_back(std::make_unique<Trailer>(dimension, *phys));
        m_trailers.back()->init(speed(), heading);
        m_trailers.back()->name += std::to_string(m_trailers.size());
        return *m_trailers.back();
    }

    //-------------------------------------------------------------------------
    //! \brief Check if the car or one of its trailer collides with another
    //! vehicle and one of its trailers.
    //-------------------------------------------------------------------------
    bool collides(Car const& other) const
    {
        sf::Vector2f p;

        // TODO: traillers collisions
        return m_shape.collides(other.shape(), p);
    }

    //-------------------------------------------------------------------------
    //! \brief Set the reference speed.
    //-------------------------------------------------------------------------
    void setRefSpeed(float const speed)
    {
        m_control.set_speed(speed);
    }

    //-------------------------------------------------------------------------
    //! \brief Set the reference steering angle.
    //-------------------------------------------------------------------------
    void setRefSteering(float const angle)
    {
        m_control.set_steering(angle);
    }

    //-------------------------------------------------------------------------
    //! \brief Lateral displacement
    //-------------------------------------------------------------------------
    //FIXME bool displacement(const float dx, const float dy)
    //{
    //    return CarTrajectory::create(dx, dy);
    //}

    //-------------------------------------------------------------------------
    //! \brief Update the trajectory, cruise control, car physics ...
    //! \param[in] dt: delta time [seconds] from the previous call.
    //-------------------------------------------------------------------------
    void update(float const dt)
    {
        m_control.update(dt);
        m_kinematic.update(m_control, dt);
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return longitudinal acceleration [meter/seconds^2].
    //--------------------------------------------------------------------------
    inline float acceleration() const
    {
        return m_kinematic.acceleration();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return longitudinal speed [meter/seconds].
    //--------------------------------------------------------------------------
    inline float speed() const
    {
        return m_kinematic.speed();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return position of the middle of the rear axle.
    //--------------------------------------------------------------------------
    inline sf::Vector2f position() const
    {
        return m_kinematic.position();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the heading [rad].
    //--------------------------------------------------------------------------
    inline float heading() const
    {
        return m_shape.heading();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return all the wheels.
    //--------------------------------------------------------------------------
    inline std::vector<Wheel> const& wheels() const
    {
        return m_shape.wheels();
    }

    //--------------------------------------------------------------------------
    //! \brief Const getter: return the nth wheel.
    //--------------------------------------------------------------------------
    inline Wheel const& wheel(CarShape::WheelName const nth) const
    {
        return m_shape.wheel(size_t(nth));
    }

    //-------------------------------------------------------------------------
    //! \brief Const getter: return the car shape.
    //-------------------------------------------------------------------------
    inline CarShape const& shape() const
    {
        return m_shape;
    }

    //-------------------------------------------------------------------------
    //! \brief Const getter: return the oriented bounding box.
    //-------------------------------------------------------------------------
    inline sf::RectangleShape const& obb() const
    {
        return m_shape.obb();
    }

    //-------------------------------------------------------------------------
    //! \brief Const getter: return all trailers attached to the car in order
    //! and starting by the one attached to the car.
    //-------------------------------------------------------------------------
    std::deque<std::unique_ptr<Trailer>> const& trailers() const
    {
        return m_trailers;
    }

    //-------------------------------------------------------------------------
    //! \brief Debug purpose only: show shape information.
    //-------------------------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, Car const& car)
    {
        return os << "Vehicle " << car.name << " {" << std::endl
                  << car.dim << std::endl
                  << car.m_shape << std::endl
                  << "}";
    }

public:

    //! \brief Car's read-only dimension.
    CarDimension const dim;

    //! \brief Car's name
    std::string name = "car";

    //! \brief Current car color. Public: to allow to change it for distinguish
    //! car between them or for showing collisions ...
    sf::Color color = sf::Color(DEFAULT_VEHICLE_COLOR);

protected:

    //! \brief Car blueprint.
    CarShape m_shape;
    //! \brief Car Physics.
    CarKinematic m_kinematic;
    //! \brief Car cruise control.
    CarControl m_control;
    //! \brief List of trailers attached to the car
    std::deque<std::unique_ptr<Trailer>> m_trailers;
};

#endif
