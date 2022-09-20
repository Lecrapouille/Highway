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
#ifndef GUI_SIMULATION_STATE_HPP
#  define GUI_SIMULATION_STATE_HPP

#  include "Application/Application.hpp"
#  include "Simulator/Simulator.hpp"
#  include "Renderer/MessageBar.hpp"
#  include <atomic>

// ****************************************************************************
//! \brief Concrete SFML Application::GUI managing the GUI for the car
//! simulation. The world coordinates are:
//!     ^ Y
//!     |
//!     +------> X
//! to follow the same computations than the document "Estimation et controle
//! pour le pilotage automatique de vehicule" by Sungwoo Choi.
// ****************************************************************************
class GUISimulation: public Application::GUI
{
public:

    //-------------------------------------------------------------------------
    //! \brief Default Constructor.
    //! \param[inout] application: the main class managing the stack of GUI.
    //! \param[in] name: the key for searching a GUI.
    //-------------------------------------------------------------------------
    GUISimulation(Application& application, const char* name);

    //-------------------------------------------------------------------------
    //! \brief Apply the zoom value.
    //-------------------------------------------------------------------------
    inline void zoom(float const value);

private: // Derived from Application::GUI

    //-------------------------------------------------------------------------
    //! \brief Draw the world and its entities.
    //-------------------------------------------------------------------------
    virtual void draw() override;

    //-------------------------------------------------------------------------
    //! \brief Update the simulation.
    //-------------------------------------------------------------------------
    virtual void update(const float dt) override;

    //-------------------------------------------------------------------------
    //! \brief Manage mouse and keyboard events.
    //-------------------------------------------------------------------------
    virtual void handleInput() override;

    //-------------------------------------------------------------------------
    //! \brief Return true if GUI is alive.
    //-------------------------------------------------------------------------
    virtual bool running() const override
    {
        return m_running && simulator.running();
    }

    //-------------------------------------------------------------------------
    //! \brief Pause the GUI.
    //-------------------------------------------------------------------------
    virtual void activate() override;

    //-------------------------------------------------------------------------
    //! \brief Unpause the GUI.
    //-------------------------------------------------------------------------
    virtual void deactivate() override;

    //-------------------------------------------------------------------------
    //! \brief Create the GUI.
    //-------------------------------------------------------------------------
    virtual void create() override;

    //-------------------------------------------------------------------------
    //! \brief Release the GUI.
    //-------------------------------------------------------------------------
    virtual void release() override;

private:

    //! \brief Halting the GUI ?
    std::atomic<bool> m_running{true};
    //! \brief For managing world coordinates, zoom and camera displacement.
    sf::View m_simulation_view;
    //! \brief For displaying the GUI.
    sf::View m_hud_view;
    //! \brief Camera zoom
    float m_zoom;
    //! \brief Mouse X,Y position within the world coordinate [meter]. You
    //! directly can measure objects [meter] by clicking with mouse in the
    //! window.
    sf::Vector2f m_mouse;
    //! \brief SFML loaded font from a TTF file.
    sf::Font m_font;

public:

    //! \brief City, cars, pedestrians simulation ... in which we will test the
    //! ego vehicle.
    Simulator simulator;
};

#endif
