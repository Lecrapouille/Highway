//=====================================================================
// https://github.com/Lecrapouille/Highway
// Highway: Open-source simulator for autonomous driving research.
// Copyright 2021 -- 2022 Quentin Quadrat <lecrapouille@gmail.com>
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
// along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#ifndef SIMULATOR_HPP
#  define SIMULATOR_HPP

#  include "Simulator/City/City.hpp"
#  include "Renderer/MessageBar.hpp"
#  include "Common/DynamicLoader.hpp"

class Renderer;
class Simulator;

// ****************************************************************************
//! \brief Class holding C functions loaded from a shared library for setting
//! and configurate simulation scenarios. This class is used by the simulator.
// ****************************************************************************
struct Scenario
{
    //! \brief C fonction returning the name of the simulation.
    std::function<const char* (void)> name = nullptr;
    //! \brief C fonction taking a City as input and allows to create the
    //! desired city (roads, parking, vehicles ...). This function returns the
    //! ego car.
    std::function<Car&(City&)> create = nullptr;
    //! \brief C function taking the Simulator as input and returns true when
    //! the simulation shall halt (simulation duration, collision, reach a given
    //! position ...).
    std::function<bool(Simulator const&)> halt = nullptr;
    //! \brief C function Reacting to external events (refered by their ID for
    //! example keyboard keys).
    std::function<void(Simulator& simulator, size_t key)> react = nullptr;
};

// ****************************************************************************
//! \brief Class managing a simulation. This class is owned by the Application
//! GUI. This class owns a City (roads, parkings ...), simulation actors
//! (vehicles ...) which are loaded by a scenario file (shared library owning
//! some special C functions used for creating and setting the simulation. See
//! for example Drive/Simulations/SimpleParking/SimpleParking.cpp).
//!
//! Idea:
//! https://www.mathworks.com/help/driving/ug/motion-planning-using-dynamic-map.html
// ****************************************************************************
class Simulator
{
private:

    // *************************************************************************
    //! \brief Helper class for loading scenarios.
    // *************************************************************************
    class ScenarioLoader : public DynamicLoader
    {
    public:

        ScenarioLoader(Scenario& scenario)
            : m_scenario(scenario)
        {}

    private:

        //----------------------------------------------------------------------
        //! \brief Concrete implementation of the callback triggered when the
        //! simulation file (shared library) has been loaded with success. This
        //! method will loaded desired C functions. In case of error an
        //! execption is thrown.
        //----------------------------------------------------------------------
        virtual void onLoading() override;

    private:

        Scenario& m_scenario;
    };

public:

    //-------------------------------------------------------------------------
    //! \brief Default constructor. Take the SFML renderer, from the Application
    //! instance, needed for drawing the simulation.
    //-------------------------------------------------------------------------
    Simulator(sf::RenderWindow& renderer);

    //-------------------------------------------------------------------------
    //! \brief Pass a text to the simulator to display it inside the messagebox
    //! widget.
    //-------------------------------------------------------------------------
    inline void messagebox(std::string const& txt, sf::Color const& color) const
    {
        m_message_bar.entry(txt, color);
    }

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
    //! \brief Callback when the GUI has started: create the city, camera ...
    //-------------------------------------------------------------------------
    void activate();

    //-------------------------------------------------------------------------
    //! \brief Callback when the GUI has ended. Reset the simulation states,
    //! remove entities: parking, cars, ego car ...
    //-------------------------------------------------------------------------
    void deactivate();
    void create();
    void release();

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
    void draw_simulation(); // FIXME const
    void draw_hud(); // FIXME const

    //--------------------------------------------------------------------------
    //! \brief Return the latest error.
    //--------------------------------------------------------------------------
    inline std::string const& error() const
    {
        return m_error;
    }

private:

    void collisions(Car& ego);

private:

    //! \brief SFML renderer needed for drawing the simulation.
    sf::RenderWindow& m_renderer;
    //! \brief
    ScenarioLoader m_loader;
    //! \brief Simulation scenario loaded from a shared library.
    Scenario m_scenario;
    //! \brief
    bool m_scenario_loaded = false;
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
    //! \brief Memorize the latest error.
    std::string m_error;
    //! \brief Display messages
    mutable MessageBar m_message_bar;
};

#endif
