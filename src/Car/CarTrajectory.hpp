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

#ifndef CAR_TRAJECTORY_HPP
#  define CAR_TRAJECTORY_HPP

#  include "Parking.hpp"
#  include <SFML/Graphics.hpp> // FIXME deplacer CarTrajectory::draw
#  include <vector>
#  include <cassert>

class Car;
class CarControl;

static const bool USE_KINEMATIC = true;
static const float VMAX = 1.0f; // [m/s]
static const float ADES = 1.0f; // [m/s/s]

class References
{
public:

    // *************************************************************************
    //! \brief Helper struct to hold a value to maintain during a given duration.
    // *************************************************************************
    struct TimedValue
    {
        //----------------------------------------------------------------------
        //! \brief param[in] v: value [generic type]
        //! \brief param[in] t: duration [generic time]
        //----------------------------------------------------------------------
        TimedValue(float v, float t)
            : value(v), time(t)
        {}

        float value;
        float time;
    };

    //--------------------------------------------------------------------------
    void clear()
    {
        m_references.clear();
    }

    //--------------------------------------------------------------------------
    void add(float const value, float const duration)
    {
        assert(duration >= 0.0f);

        float time = m_references.empty() ? 0.0f : m_references.back().time;
        m_references.push_back(TimedValue(value, time + duration));
    }

    //--------------------------------------------------------------------------
    float get(float const time)
    {
        if (m_references.size() == 0u)
        {
            return 0.0f;
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

        return 0.0f;
    }

private:

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
    virtual void update(CarControl& control, float const dt);
    virtual void draw(sf::RenderTarget& /*target*/, sf::RenderStates /*states*/) const {};

protected:

    //! \brief Integration time
    float m_time = 0.0f;
    //! \brief Timed reference for the car accelerations.
    References m_accelerations;
    //! \brief Timed reference for the car speeds.
    References m_speeds;
    //! \brief Timed reference for front wheel angles.
    References m_steerings;
};

// *************************************************************************
//! \brief Compute the trajectory allowing the car to park. For parallel
//! parking only!
// *************************************************************************
class ParallelTrajectory: public CarTrajectory
{
public:

    virtual bool init(Car& car, Parking const& parking, bool const entering) override;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:

    bool computePathPlanning(Car const& car, Parking const& parking, bool const entering);
    // Max valocity [m/s]
    // Desired acceleration [m/s/s]
    void generateReferenceTrajectory(Car const& car, bool const entering, float const vmax, float const ades);

private:

    float Rimin, Remin, Rwmin;
    //! \brief X-Y world coordinates:
    // s: initial car position
    // f: final car position
    // s: starting position for turning
    // c1: Center of the circle 1
    // c2: Center of the circle 2
    // t: tangential intersection point between C1 and C2
    float Xc1, Yc1, Xc2, Yc2, Xt, Yt, Xs, Ys, Xi, Yi, Xf, Yf;
    //! \brief Minimal central angle for making the turn.
    float min_central_angle;
};

// *************************************************************************
//! \brief
// *************************************************************************
class DiagonalTrajectory: public CarTrajectory
{
public:

    virtual bool init(Car& car, Parking const& parking, bool const entering) override;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:

    bool computePathPlanning(Car const& car, Parking const& parking, bool const entering);
    void generateReferenceTrajectory(Car const& car, bool const entering, float const vmax, float const ades);

private:

    // rayon pour sortir
    float Rin1;
    float theta1;
    float beta1; // angle braquage

    float Rin2;
    float theta2;
    float beta2; // angle braquage

    float Xi, Yi, Xdm, Ydm, Xc, Yc, dl;
};

// *************************************************************************
//! \brief
// *************************************************************************
class PerpTrajectory: public CarTrajectory
{
public:

    virtual bool init(Car& car, Parking const& parking, bool const entering) override;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:

    bool computePathPlanning(Car const& car, Parking const& parking, bool const entering);
    // Max valocity [m/s]
    // Desired acceleration [m/s/s]
    void generateReferenceTrajectory(Car const& car, bool const entering, float const vmax, float const ades);

private:

    float Rmin, Rwmin;
    float Xi, Yi, Lz, Xe, Ye;
};

#endif
