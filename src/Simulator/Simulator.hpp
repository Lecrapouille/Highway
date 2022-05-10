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

#  include "Simulator/City/City.hpp"
#  include "Simulator/Simulation.hpp"
#  include "Renderer/MessageBar.hpp"

class Renderer;

// ****************************************************************************
//! \brief Class managing a simulation. This class is owned by the Application
//! GUI. This class owns a City (roads, parkings ...), simulation actors
//! (vehicles ...) which are loaded by a scenario file (shared library owning
//! some special C functions used for creating and setting the simulation. See
//! for example Drive/Simulations/SimpleParking/SimpleParking.cpp).
//!
//! Idea: https://www.mathworks.com/help/driving/ug/motion-planning-using-dynamic-map.html
// ****************************************************************************
class Simulator
{
public:

    //-------------------------------------------------------------------------
    //! \brief Default constructor. Take the SFML renderer, from the Application
    //! instance, needed for drawing the simulation.
    //-------------------------------------------------------------------------
    Simulator(sf::RenderWindow& renderer);

    //-------------------------------------------------------------------------
    //! \brief Load a simulation file (a shared library file holding functions
    //! needed for defining a scenario).
    //! \param[in] libpath the path to the shared libray compiled against this
    //!   code, holding functions needed for defining a scenario (see
    //!   Drive/Simulations/SimpleParking/SimpleParking.cpp for example).
    //! \return Return true if the shared library has been loaded with success
    //!   else return false.
    //-------------------------------------------------------------------------
    bool load(const char* libpath);

    //-------------------------------------------------------------------------
    //! \brief Load a scenario (the structure holding functions loaded from a
    //! simulation file, as shared library).
    //! \param[in] scenario structure
    //! \return Return true if the shared library has been loaded with success
    //!   else return false.
    //-------------------------------------------------------------------------
    bool load(Scenario const& scenario);

    //-------------------------------------------------------------------------
    //! \brief Reload the simulation file (load() should have been called
    //! previously).
    //-------------------------------------------------------------------------
    bool reload();

    //-------------------------------------------------------------------------
    //! \brief Make the simulator reacts to the given event ID.
    //-------------------------------------------------------------------------
    void reactTo(size_t key);

    //-------------------------------------------------------------------------
    //! \brief Return the simulation elapsed time.
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
    //! \brief Return the ego car. The ego car shall exist !
    //-------------------------------------------------------------------------
    inline Car& ego()
    {
        assert(m_ego != nullptr);
        return *m_ego;
    }

    //-------------------------------------------------------------------------
    //! \brief Return the ego car. The ego car shall exist !
    //-------------------------------------------------------------------------
    inline Car const& ego() const
    {
        assert(m_ego != nullptr);
        return *m_ego;
    }

    //-------------------------------------------------------------------------
    //! \brief Make the camera follows the given car.
    //-------------------------------------------------------------------------
    inline void follow(Car* car)
    {
        m_follow = car;
    }

    //-------------------------------------------------------------------------
    //! \brief Return the position of the camera 'looking at' position.
    //-------------------------------------------------------------------------
    inline sf::Vector2f const& camera() const
    {
        return m_camera;
    }

    //-------------------------------------------------------------------------
    //! \brief Pass a text to the simulator to display it inside the messagebox
    //! widget.
    //-------------------------------------------------------------------------
    inline void messagebox(std::string const& txt, sf::Color const& color) const
    {
        message_bar.entry(txt, color);
    }

    //-------------------------------------------------------------------------
    //! \brief Callback when the GUI has started: create the city, camera ...
    //-------------------------------------------------------------------------
    void activate();

    //-------------------------------------------------------------------------
    //! \brief Callback when the GUI has ended. Reset the simulation states,
    //! remove entities: parking, cars, ego car ...
    //-------------------------------------------------------------------------
    void deactivate();

    //-------------------------------------------------------------------------
    //! \brief Callback when the application needs to know if the GUI shall
    //! halt or continue.
    //-------------------------------------------------------------------------
    bool running() const;

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
    void hud();

private:

    void showCollisions(Car& ego);

public:

    //! \brief Display messages
    mutable MessageBar message_bar;

protected:

    //! \brief SFML renderer needed for drawing the simulation.
    sf::RenderWindow& m_renderer;
    //! \brief The city we want to simulated with its roads, parkings, cars ...
    City m_city;
    //! \brief Make the camera follow the given car.
    Car* m_follow = nullptr;
    //! \brief The ego car.
    Car* m_ego = nullptr;
    //! \brief Camera position tracking a vehicle.
    sf::Vector2f m_camera;
    //! \brief Simulation time.
    sf::Clock m_time;
    //! \brief Simulation scenario loaded from a shared library.
    Simulation m_simulation;
};

#endif
