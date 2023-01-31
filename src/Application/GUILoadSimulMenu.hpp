//==============================================================================
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
//==============================================================================
#ifndef GUI_LOAD_SIMULATIONS_MENU_HPP
#  define GUI_LOAD_SIMULATIONS_MENU_HPP

#  include "Application/GUISimulation.hpp"
#  include <atomic>

// ****************************************************************************
//! \brief Main menu of the simulator. Offer the possibility to load a scenario.
// ****************************************************************************
class GUILoadSimulMenu: public Application::GUI
{
public:

    //-------------------------------------------------------------------------
    //! \brief Default Constructor.
    //! \param[inout] application: the main class managing the stack of GUI.
    //-------------------------------------------------------------------------
    GUILoadSimulMenu(Application& application, std::string const& name);

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
    //! \brief Pause the simulation.
    //-------------------------------------------------------------------------
    virtual void onActivate() override;

    //-------------------------------------------------------------------------
    //! \brief Unpause the simulation.
    //-------------------------------------------------------------------------
    virtual void onDeactivate() override;

    //-------------------------------------------------------------------------
    //! \brief Create the simulation.
    //-------------------------------------------------------------------------
    virtual void onCreate() override;

    //-------------------------------------------------------------------------
    //! \brief Release the simulation.
    //-------------------------------------------------------------------------
    virtual void onRelease() override;

private:

    void createListScenarios();

private:

    struct ScenarioEntry
    {
        fs::path libpath;
        fs::path filename;
        std::string brief;
    };

    //! \brief Camera's view
    sf::View m_view;
    //! \brief Display messages
    sf::Text m_text;
    //! \brief List of scenarios
    std::vector<ScenarioEntry> m_scenarios;
    //! \brief Selector of menu
    size_t m_cursor = 0u;
};

#endif
