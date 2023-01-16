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

#ifndef CAR_TRAJECTORIES_HPP
#  define CAR_TRAJECTORIES_HPP

#  include "Simulator/City/Parking.hpp"
#  include <SFML/Graphics.hpp> // FIXME deplacer CarTrajectory::draw
#  include <vector>

class Car;
class VehicleControl;

static const bool USE_KINEMATIC = true;

template<class T>
class References
{
private:

    // *************************************************************************
    //! \brief Helper struct to hold a value to maintain during a given duration.
    // *************************************************************************
    struct TimedValue
    {
        //----------------------------------------------------------------------
        //! \brief param[in] v: value [generic type]
        //! \brief param[in] t: duration [generic time]
        //----------------------------------------------------------------------
        TimedValue(T v, Second t)
            : value(v), time(t)
        {}

        T value;
        Second time;
    };

public:

    //--------------------------------------------------------------------------
    void clear()
    {
        m_references.clear();
    }

    //--------------------------------------------------------------------------
    void add(T const value, Second const duration)
    {
        assert(duration >= 0.0_s);

        Second time = m_references.empty() ? 0.0_s : m_references.back().time;
        m_references.push_back(TimedValue(value, time + duration));
    }

    //--------------------------------------------------------------------------
    T get(Second const time)
    {
        if (m_references.size() == 0u)
        {
            return T(0.0);
        }

        const size_t N = m_references.size() - 1u;
        if (time <= m_references[N].time)
        {
            for (size_t i = 0u; i <= N; ++i)
            {
                if (time < m_references[i].time)
                {
                    return m_references[i].value;
                }
            }
        }

        return T(0.0);
    }

    bool end(Second const time)
    {
        return m_references.empty() || (time >= m_references.back().time);
    }

protected:

    std::vector<TimedValue> m_references;
};

// *************************************************************************
//! \brief
// *************************************************************************
class CarTrajectory
{
public:

    using Ptr = std::unique_ptr<CarTrajectory>;
    static CarTrajectory::Ptr create(Parking::Type const type);

    virtual ~CarTrajectory() = default;
    virtual bool init(Car& car, Parking const& parking, bool const entering) = 0;
    //! \brief
    //! \return false if no need to update (end of references)
    virtual bool update(Car& car, Second const dt);
    virtual void draw(sf::RenderTarget& /*target*/, sf::RenderStates /*states*/) const {};

protected:

    //! \brief Integration time
    Second m_time = 0.0_s;
    //! \brief Timed reference for the car accelerations.
    References<MeterPerSecondSquared> m_accelerations;
    //! \brief Timed reference for the car speeds.
    References<MeterPerSecond> m_speeds;
    //! \brief Timed reference for front wheel angles.
    References<Radian> m_steerings;
};

#endif
