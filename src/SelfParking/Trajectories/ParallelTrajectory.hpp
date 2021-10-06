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


#ifndef PARALLEL_TRAJECTORY_HPP
#  define PARALLEL_TRAJECTORY_HPP

#  include "SelfParking/Trajectories/Trajectory.hpp"

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

    size_t computePath(Car const& car, Parking const& parking);
    void generateReferences(Car const& car, Parking const& parking,
                            float const VMAX, float const ADES);
private:

    //! \brief Minimal turning radius for the internal point of the ego car.
    float Rimin;
    //! \brief Minimal turning radius for the external point of the ego car.
    float Remin;
    //! \brief Minimal turning radius for the external point of the ego car.
    float Rwmin;
    //! \brief Minimal turning radius for the external point of the ego car.
    float Lmin;
    //! \brief Number needed of maneuvers for parking the car (change of gear)
    size_t m_maneuvers = 0u;
    //! \brief X-Y world coordinates of the middle rear axle of the ego car.
    std::vector<sf::Vector2f> Em;
    //! \brief X-Y world coordinates of the immedite center of rotations.
    std::vector<sf::Vector2f> C;
    //! \brief X-Y world coordinates of the initial position of the middle of
    //! rear axle of the ego car.
    float Xi, Yi;
    //! \brief X-Y world coordinates of the starting position of the middle of
    //! rear axle of the ego car for doing the first turning.
    float Xs, Ys;
    //! \brief X-Y world coordinates of the starting position of the middle of
    //! rear axle of the ego car at the end of the maneuver.
    float Xf, Yf;
    //! \brief X-Y world coordinates of the tangential point of the two final
    //! turns.
    float Xt, Yt;
    //! \brief Memorize angles
    std::vector<float> theta_E, theta_t, theta_s, theta_p, theta_g, theta_sum;
    //! \brief Memorize Turning radius
    std::vector<float> Rrg;
};

#endif
