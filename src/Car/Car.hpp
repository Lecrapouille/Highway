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
//#  include "CarControl.hpp"
//#  include "CarShape.hpp"
#  include "CarPhysics.hpp"
#  include "CarTrajectory.hpp"
#  include <memory>

class Car
{
public:

    Car(CarDimension const& dim_)
        : dim(dim_), m_kinematic(dim_)
    {}

    Car(const char* model)
        : Car(CarDimensions::get(model))
    {}

    inline void init(sf::Vector2f const& position, float const speed,
                     float const heading, float const steering)
    {
        m_kinematic.init(position, speed, heading, steering);
    }

    float estimate_parking_length() { return 10.0f; }

    bool park(sf::Vector2f const& destination)
    {
        m_trajectory = std::make_unique<ParallelTrajectory>();
        return m_trajectory->init(*this, destination);
    }

    void update(float const dt)
    {
        if (m_trajectory == nullptr)
            return ;

        m_trajectory->update(m_control, dt);
        m_control.update(dt);
        m_kinematic.update(dt, m_control);
    }

    CarTrajectory const& trajectory() const
    {
        assert(m_trajectory != nullptr);
        return *m_trajectory;
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
        return shape().heading();
    }

    inline std::array<Wheel, 4> const& wheels() const
    {
        return shape().wheels();
    }

    inline Wheel const& wheel(Wheel::Type const i) const
    {
        return shape().wheel(i);
    }

    inline CarShape const& shape() const
    {
        return m_kinematic.shape();
    }

    friend std::ostream& operator<<(std::ostream& os, Car const& car)
    {
        return os << "Car {" << std::endl
                  << car.dim << std::endl
                  << car.shape() << std::endl
                  << "}";
    }

public:

    CarDimension const dim;

private:

    CarKinematic m_kinematic;
    CarControl m_control;
    std::unique_ptr<CarTrajectory> m_trajectory = nullptr;
};

#endif
