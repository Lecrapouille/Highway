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

#pragma once

#  include "Core/Common/FileSystem.hpp"
#  include "Core/Simulator/Monitoring.hpp"
#  include "Core/Simulator/City/City.hpp"
#  include "Core/Scenario/Scenario.hpp"
//#  include "Vehicle/ECU.hpp"
//#  include "Application/MessageBar.hpp" // Ne pas dependre du renderer !!!!


class Renderer;

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
public:

    //-------------------------------------------------------------------------
    //! \brief Default constructor. Take the SFML renderer, from the Application
    //! instance, needed for drawing the simulation.
    //-------------------------------------------------------------------------
    Simulator();//sf::RenderWindow& renderer, MessageBar& message_bar);

    //-------------------------------------------------------------------------
    //! \brief Load a simulation file: a shared library file holding functions
    //! needed for defining a scenario. Call this method as create or init
    //! entry point.
    //! \param[in] libpath the path to the shared libray compiled against this
    //!   code, holding functions needed for defining a scenario (see
    //!   Drive/Simulations/SimpleParking/SimpleParking.cpp for example).
    //! \return Return true if the shared library has been loaded with success
    //!   else return false and use \c error() to get the reason.
    //-------------------------------------------------------------------------
    bool load(fs::path const& libpath);

    //-------------------------------------------------------------------------
    //! \brief Restart the simulation. Shall be called after \c load().
    //-------------------------------------------------------------------------
    inline bool restart() { return init(); }

    //-------------------------------------------------------------------------
    //! \brief Pause or unpause the simulation.
    //-------------------------------------------------------------------------
    void pause(bool const state);

    //-------------------------------------------------------------------------
    //! \brief Get info if the simulation is in pause or not.
    //-------------------------------------------------------------------------
    inline bool pause() const { return m_pause; }

    //-------------------------------------------------------------------------
    //! \brief When GUI triggered the onActivate().
    //-------------------------------------------------------------------------
    void activate();

    //-------------------------------------------------------------------------
    //! \brief When GUI triggered the onDectivate().
    //-------------------------------------------------------------------------
    void deactivate();

    //-------------------------------------------------------------------------
    //! \brief Make the simulator reacts to the given input event ID.
    //-------------------------------------------------------------------------
    void reacts(size_t key);

    //-------------------------------------------------------------------------
    //! \brief To know if the simulation has halted (conditions are depicted by
    //! the scenario) or is continuing. In case where the simulation has halted
    //! the GUI should close i.e.
    //-------------------------------------------------------------------------
    bool continuing() const;

    //-------------------------------------------------------------------------
    //! \brief Update the simuation states.
    //! \param[in] dt: delta time in seconds from the previous call.
    //-------------------------------------------------------------------------
    void update(const Second dt);

    //-------------------------------------------------------------------------
    //! \brief When GUI triggered the onRelease().
    //-------------------------------------------------------------------------
    void release();

    //-------------------------------------------------------------------------
    //! \brief Return the city
    //-------------------------------------------------------------------------
    inline City const& city()
    {
        return m_city;
    }

    //-------------------------------------------------------------------------
    //! \brief Return the ego car.
    //-------------------------------------------------------------------------
    inline Car& ego()
    {
        return *m_ego;
    }

    //-------------------------------------------------------------------------
    //! \brief Return the ego car.
    //-------------------------------------------------------------------------
    inline Car const& ego() const
    {
        return *m_ego;
    }

    //-------------------------------------------------------------------------
    //! \brief Make the camera follows the given car.
    //-------------------------------------------------------------------------
    inline void follow(Car& car)
    {
        m_follow = &car;
    }

    //-------------------------------------------------------------------------
    //! \brief Return the position of the camera 'looking at' position.
    //-------------------------------------------------------------------------
    inline sf::Vector2f const& camera() const
    {
        return m_camera;
    }

    //-------------------------------------------------------------------------
    //! \brief Return the simulation elapsed time.
    //-------------------------------------------------------------------------
    inline Second elapsedTime() const
    {
        return Second((m_elpased_time + m_clock.getElapsedTime()).asSeconds());
    }

    //-------------------------------------------------------------------------
    //! \brief Pass a text to the simulator to display it inside the messagebox
    //! widget.
    //-------------------------------------------------------------------------
    inline void messagebar(std::string const& txt, sf::Color const& color) const
    {
         LOGI("Scenario: %s", txt.c_str());
        //m_message_bar.entry(txt, color);
    }

    //--------------------------------------------------------------------------
    //! \brief Return the latest error.
    //--------------------------------------------------------------------------
    inline std::string const& error() const
    {
        return m_error;
    }

    //--------------------------------------------------------------------------
    //! \brief Return the scenario name.
    //--------------------------------------------------------------------------
    inline std::string scenarioName() const
    {
        return m_scenario.name();
    }

private:

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    bool init();

    //-------------------------------------------------------------------------
    //! \brief Reload the scenario if it has changed.
    //-------------------------------------------------------------------------
    bool autoreload();

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    void collisions(Car& ego);

//private: // Inheritance from ECU::Listener
//
//    virtual void onMessageToLog(std::string const& message) const override
//    {
//        messagebar(message, sf::Color::Yellow);
//    }

public:

    //! \brief Record simulation states.
    //! FIXME allow to have several monitoring systems
    Monitor monitor;

private:

    //! \brief Simulation scenario loaded from a shared library.
    Scenario m_scenario;
    //! \brief The simulated city (with its roads, parkings, cars,
    //! pedestrians ...)
    City m_city;
    //! \brief The ego car we want to simulate.
    Car* m_ego = nullptr; // FIXME remove the pointer!
    //! \brief Memorize the camera position.
    sf::Vector2f m_camera;
    //! \brief Camera follow the given car.
    Car* m_follow = nullptr;
    //! \brief Current elapsed time of the simulation.
    sf::Clock m_clock;
    //! \brief Total elapsed time of the simulation.
    sf::Time m_elpased_time;
    //! \brief Freeze the simulation.
    bool m_pause = false;
    //! \brief Memorize the latest error.
    std::string m_error;
};