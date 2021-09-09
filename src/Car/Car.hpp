// 2021 Quentin Quadrat lecrapouille@gmail.com
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

#ifndef CAR_HPP
#  define CAR_HPP

#  include "Dimensions.hpp"
#  include "TurningRadius.hpp"
#  include "CarPhysics.hpp"
#  include "CarTrajectory.hpp"
#  include <memory>
#  include <deque>

#define DEFAULT_CAR_COLOR 178, 174, 174

class Trailer
{
public:

    Trailer(TrailerDimension const& dim_, CarPhysics& next)
        : dim(dim_), m_kinematic(dim_, next)
    {}

    /*void attach(CarPhysics& kinematic)
    {
        m_kinematic.attach(kinematic);
        }*/

    inline void init(float const speed, float const heading)
    {
        m_kinematic.init(speed, heading);
    }

    void update(CarControl control, float const dt)
    {
        m_kinematic.update(control, dt);
    }

    inline float acceleration() const
    {
        return m_kinematic.acceleration();
    }

    inline float speed() const
    {
        return m_kinematic.speed();
    }

    inline sf::Vector2f position() const
    {
        return m_kinematic.position();
    }

    inline float heading() const
    {
        return shape<TrailerShape>().heading();
    }

    inline std::array<Wheel, 2> const& wheels() const
    {
        return shape<TrailerShape>().wheels();
    }

    inline Wheel const& wheel(TrailerShape::WheelType const i) const
    {
        return shape<TrailerShape>().wheel(i);
    }

    template<class T>
    inline T const& shape() const
    {
        return *reinterpret_cast<const T*>(&m_kinematic.shape());
    }

    TrailerKinematic /*const*/& kinematic() /*const*/
    {
        return m_kinematic;
    }

public:

    TrailerDimension const dim;

private:

    TrailerKinematic m_kinematic;
};


class Car
{
public:

    Car(CarDimension const& dim_)
        : dim(dim_), m_kinematic(dim_)
    {}

    Car(const char* model)
        : Car(CarDimensions::get(model))
    {}

    void attach(TrailerDimension const& dim, const float heading)
    {
        CarPhysics* phys;

        if (m_trailers.empty())
        {
            phys = &m_kinematic;
        }
        else
        {
            phys = &(m_trailers.back()->kinematic());
        }

        m_trailers.push_back(std::make_unique<Trailer>(dim, *phys));
        m_trailers.back()->init(speed(), heading);
    }

    bool intersects(Car const& other) const
    {
        // FIXME: traillers collisions

        return shape<CarShape>().intersects(other.shape<CarShape>());
    }

    inline void init(sf::Vector2f const& position, float const speed,
                     float const heading, float const steering)
    {
        m_kinematic.init(position, speed, heading, steering);
        for (auto& it: m_trailers)
        {
            it->init(this->speed(), 0.0f); // TODO memorize headings
        }
    }

    float estimate_parking_length() { return 10.0f; } // FIXME

    bool park(Parking const& parking)
    {
        m_trajectory = CarTrajectory::create(parking.type);
        return m_trajectory->init(*this, parking, parking.empty());
    }

    void update(float const dt)
    {
        if (m_trajectory == nullptr)
            return ;

        m_trajectory->update(m_control, dt);
        m_control.update(dt);
        m_kinematic.update(m_control, dt);
    }

    CarTrajectory const& trajectory() const
    {
        assert(m_trajectory != nullptr);
        return *m_trajectory;
    }

    bool hasTrajectory() const
    {
        return m_trajectory != nullptr;
    }

    inline float acceleration() const
    {
        return m_kinematic.acceleration();
    }

    inline float speed() const
    {
        return m_kinematic.speed();
    }

    inline sf::Vector2f position() const
    {
        return m_kinematic.position();
    }

    inline float heading() const
    {
        return shape<CarShape>().heading();
    }

    inline std::array<Wheel, 4> const& wheels() const
    {
        return shape<CarShape>().wheels();
    }

    inline Wheel const& wheel(CarShape::WheelType const i) const
    {
        return shape<CarShape>().wheel(i);
    }

    template<class T>
    inline T const& shape() const
    {
        return *reinterpret_cast<const T*>(&m_kinematic.shape());
    }

    friend std::ostream& operator<<(std::ostream& os, Car const& car)
    {
        return os << "Car {" << std::endl
                  << car.dim << std::endl
                  << car.shape<CarShape>() << std::endl
                  << "}";
    }

    std::deque<std::unique_ptr<Trailer>> const& trailers() const
    {
        return m_trailers;
    }

public:

    CarDimension const dim;
    sf::Color color = sf::Color(DEFAULT_CAR_COLOR);

private:

    CarKinematic m_kinematic;
    CarControl m_control;
    std::unique_ptr<CarTrajectory> m_trajectory = nullptr;
    std::deque<std::unique_ptr<Trailer>> m_trailers;
};

#endif
