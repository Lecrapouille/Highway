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
    //! \param[in] scenario_path the path to the simulation file (shared lib)
    //! if set empty then an hello simulation is given.
    //-------------------------------------------------------------------------
    GUISimulation(Application& application, std::string const& name,
                  std::string const& scenario_path = {});

    //-------------------------------------------------------------------------
    //! \brief Return the zoom level.
    //-------------------------------------------------------------------------
    inline float zoom_level() const { return m_zoom_level; }

private:

    //-------------------------------------------------------------------------
    //! \brief Apply the zoom value.
    //-------------------------------------------------------------------------
    inline void applyZoom(float const value);

    //-------------------------------------------------------------------------
    //! \brief Pass a text to the simulator to display it inside the messagebox
    //! widget.
    //-------------------------------------------------------------------------
    inline void messagebox(std::string const& txt, sf::Color const& color) const
    {
        m_message_bar.entry(txt, color);
    }

private: // Derived from Application::GUI

    //-------------------------------------------------------------------------
    //! \brief Draw the world and its entities.
    //-------------------------------------------------------------------------
    virtual void onDraw() override;

    //-------------------------------------------------------------------------
    //! \brief Update the simulation.
    //-------------------------------------------------------------------------
    virtual void onUpdate(const Second dt) override;

    //-------------------------------------------------------------------------
    //! \brief Manage mouse and keyboard events.
    //-------------------------------------------------------------------------
    virtual void onHandleInput() override;

    //-------------------------------------------------------------------------
    //! \brief Pause the GUI.
    //-------------------------------------------------------------------------
    virtual void onActivate() override;

    //-------------------------------------------------------------------------
    //! \brief Unpause the GUI.
    //-------------------------------------------------------------------------
    virtual void onDeactivate() override;

    //-------------------------------------------------------------------------
    //! \brief Create the GUI.
    //-------------------------------------------------------------------------
    virtual void onCreate() override;

    //-------------------------------------------------------------------------
    //! \brief Release the GUI.
    //-------------------------------------------------------------------------
    virtual void onRelease() override;

private:

    //! \brief For managing world coordinates, zoom and camera displacement.
    sf::View m_simulation_view;
    //! \brief For displaying the interface.
    sf::View m_interface_view;
    //! \brief Camera zoom
    float m_zoom_level = 1.0f;
    //! \brief Mouse X,Y position within the world coordinate [meter]. You
    //! directly can measure objects [meter] by clicking with mouse in the
    //! window.
    sf::Vector2<Meter> m_mouse;
    //! \brief SFML loaded font from a TTF file.
    sf::Font m_font;
    //! \brief Display info or error messages.
    mutable MessageBar m_message_bar;

public:

    //! \brief Path to the scenario file to load. Can be empty.
    std::string m_scenario_path;
    //! \brief City, cars, pedestrians simulation ... in which we will test the
    //! ego vehicle.
    Simulator simulator;
};

#endif
