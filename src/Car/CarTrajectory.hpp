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

#ifndef CAR_TRAJECTORY_HPP
#  define CAR_TRAJECTORY_HPP

#  include <SFML/Graphics.hpp> // FIXME deplacer CarTrajectory::draw
#  include <vector>
#  include <cassert>

class Car;
class CarControl;

class References
{
public:

    // *************************************************************************
    //! \brief Helper struct to hold a temporal value used for defining a
    //! trajectory.
    // *************************************************************************
    struct Pair
    {
        Pair(float d, float t)
            : data(d), time(t)
        {}

        float data;
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
        m_references.push_back(Pair(value, time + duration));
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
                    return m_references[i].data;
                }
            }
        }

        return 0.0f;
    }

private:

    std::vector<Pair> m_references;
};

// *************************************************************************
//! \brief
// *************************************************************************
class CarTrajectory
{
public:

    virtual ~CarTrajectory() = default;
    void update(CarControl& control, float const dt);
    virtual bool init(Car& car, sf::Vector2f const& destination) = 0;
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

    virtual bool init(Car& car, sf::Vector2f const& destination) override;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:

    bool computePathPlanning(Car const& car, sf::Vector2f const& destination);
    // Max valocity [m/s]
    // Desired acceleration [m/s/s]
    void generateReferenceTrajectory(Car const& car, float const vmax, float const ades);

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

#endif
