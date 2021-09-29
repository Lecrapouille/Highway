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

    bool inPath1Trial(Car const& car, Parking const& parking);
    bool inPath2Trials(Car const& car, Parking const& parking);
    //bool outPath1Trial(Car const& car, Parking const& parking);
    //bool outPath2Trials(Car const& car, Parking const& parking);

    // Max valocity [m/s]
    // Desired acceleration [m/s/s]
    void inRef1Trial(Car const& car, float const VMAX, float const ADES);
    void inRef2Trials(Car const& car, float const VMAX, float const ADES);
    //void outRef1Trial(Car const& car, float const VMAX, float const ADES);
    //void outRef2Trials(Car const& car, float const VMAX, float const ADES);

private:

    size_t m_trials = 0u;
    float Rimin, Remin, Rwmin;
    //! \brief X-Y world coordinates:
    // s: initial car position
    // f: final car position
    // s: starting position for turning
    // c1: Center of the circle 1
    // c2: Center of the circle 2
    // t: tangential intersection point between C1 and C2
    float Xc1, Yc1, Xc2, Yc2, Xt, Yt, Xs, Ys, Xi, Yi, Xf, Yf;

    float Xc3, Yc3, Xc4, Yc4, theta_t1, theta_s, theta_E1, theta_E2, theta_E3, theta_Ef, theta_sum1;
    float Xem0, Yem0, Xem1, Yem1, Xem2, Yem2, theta_sum2, theta_p, theta_g, Rrg;

    //! \brief Minimal central angle for making the turn.
    float min_central_angle;
};

#endif
