// 2021 -- 2022 Quentin Quadrat quentin.quadrat@gmail.com
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

#ifndef SIMULATOR_HPP
#  define SIMULATOR_HPP

#  include "City/City.hpp"

class Renderer;

// TODO:
// Halt condition: simulation duration, ego speed == 0 ...
// show grid: https://www.mathworks.com/help/driving/ug/motion-planning-using-dynamic-map.html

// ****************************************************************************
//! \brief Class managing car simulation.
// ****************************************************************************
class Simulator
{
public:

    typedef std::function<SelfParkingCar&(City&)> CreateCity;
    typedef std::function<bool(Simulator const&)> HaltCondition;

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    Simulator(sf::RenderWindow& renderer);

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    void createSimulation(CreateCity&& create, HaltCondition&& halt)
    {
       m_create_city = create;
       m_halt_condition = halt;
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    void reactTo(size_t key)
    {
       if (m_ego != nullptr)
         m_ego->reactTo(key);
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    inline sf::Time elapsedTime() const
    {
        return m_time.getElapsedTime();
    }

    //-------------------------------------------------------------------------
    //! \brief Convert Window's X-Y position [pixel] to world's X-Y position
    //! [meter].
    //! \param[in] p: position in the windows [pixel].
    //! \return position in the world [meter].
    //-------------------------------------------------------------------------
    inline sf::Vector2f pixel2world(sf::Vector2i const& p)
    {
        return m_renderer.mapPixelToCoords(p);
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    void activate();

    //-------------------------------------------------------------------------
    //! \brief Reset the simulation states, remove entities: parking, cars, ego
    //! car ...
    //-------------------------------------------------------------------------
    void deactivate();

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    inline void follow(Car* car)
    {
       m_follow = car;
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    inline sf::Vector2f const& camera() const
    {
       return m_camera;
    }

    //-------------------------------------------------------------------------
    //! \brief
    //-------------------------------------------------------------------------
    inline bool isRunning() const
    {
       return m_halt_condition(*this);
    }

    //-------------------------------------------------------------------------
    //! \brief Update the simuation states.
    //! \param[in] dt: delta time in seconds from the previous call.
    //-------------------------------------------------------------------------
    void update(const float dt);

    //-------------------------------------------------------------------------
    //! \brief Draw the world and its entities (cars, parkings ...)
    //! \param[in] renderer: SFML renderer (window)
    //! \param[in] view: SFML view.
    //-------------------------------------------------------------------------
    void draw(); // FIXME const

private:

void showCollisions(Car& ego);

protected:

    //! \brief
    sf::RenderWindow& m_renderer;
    //! \brief
    City m_city;
    //! \brief Make the camera follow the given car
    Car* m_follow = nullptr;
SelfParkingCar* m_ego = nullptr;
    //! \brief Camera position
    sf::Vector2f m_camera;
    //! \brief Simulation time
    sf::Clock m_time;
    //! \brief
    CreateCity m_create_city;
    //! \brief
    HaltCondition m_halt_condition;
};

#endif
