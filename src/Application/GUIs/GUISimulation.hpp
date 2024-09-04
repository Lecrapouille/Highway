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

#  include "Application/Application.hpp"
#  include "Application/Renderer/MessageBar.hpp"
#  include "Core/Simulator/Simulator.hpp"
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
    inline void messagebar(std::string const& txt, sf::Color const& color) const
    {
        m_message_bar.entry(txt, color);
    }

    //-------------------------------------------------------------------------
    //! \brief Convert the screen coordinate [pixel] to world coordinate [meter].
    //! \note Beware of the current view: be sure to be in the simulation view
    //! (meter) and not in the HUD view (pixel). The returned value would be
    //! invalid. Therefore this function is avalaible in \c drawSimulation().
    //! \param[in] p: position in the windows X-Y position [pixel].
    //! \return position in the world X-Y position [meter].
    //-------------------------------------------------------------------------
    inline sf::Vector2<Meter> pixel2world(sf::Vector2i const& p)
    {
        const sf::Vector2f w = m_renderer.mapPixelToCoords(p);
        return { Meter(w.x), Meter(w.y) };
    }

    //-------------------------------------------------------------------------
    //! \brief Convert the screen coordinate [pixel] to world coordinate [meter].
    //! \note Beware of the current view: be sure to be in the simulation view
    //! (meter) and not in the HUD view (pixel). The returned value would be
    //! invalid. Therefore this function is avalaible in \c drawSimulation().
    //! \param[in] p: position in the windows X-Y position [pixel].
    //! \return position in the world X-Y position [meter].
    //-------------------------------------------------------------------------
    inline sf::Vector2i world2pixel(sf::Vector2<Meter> const& p)
    {
        return m_renderer.mapCoordsToPixel(
            sf::Vector2f(float(p.x.value()), float(p.y.value())));
    }

    //-------------------------------------------------------------------------
    //! \brief Draw the world, city, its entities (cars, parkings ...) and the
    //! graphical interface.
    //-------------------------------------------------------------------------
    void drawSimulation();

    //-------------------------------------------------------------------------
    //! \brief Draw the Head Up Display.
    //-------------------------------------------------------------------------
    void drawHUD();

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

    enum State { Running, Closing };

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
    //! \brief
    GUISimulation::State m_state = GUISimulation::State::Running;

public:

    //! \brief Path to the scenario file to load. Can be empty.
    std::string m_scenario_path;
    //! \brief City, cars, pedestrians simulation ... in which we will test the
    //! ego vehicle.
    Simulator simulator;
};