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
#ifndef GUI_MAIN_MENU_HPP
#  define GUI_MAIN_MENU_HPP

#  include "Application/Application.hpp"
#  include <atomic>

// ****************************************************************************
//! \brief Main menu of the simulator. Offer the possibility to load a scenario.
// ****************************************************************************
class GUIMainMenu: public Application::GUI
{
public:

    //-------------------------------------------------------------------------
    //! \brief Default Constructor.
    //! \param[inout] application: the main class managing the stack of GUI.
    //-------------------------------------------------------------------------
    GUIMainMenu(Application& application, const char* name);

private: // Derived from Application::GUI

    //-------------------------------------------------------------------------
    //! \brief Draw the world and its entities.
    //-------------------------------------------------------------------------
    virtual void onDraw() override;

    //-------------------------------------------------------------------------
    //! \brief Update the simulation.
    //-------------------------------------------------------------------------
    virtual void onUpdate(const float dt) override;

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

    //! \brief Camera's view
    sf::View m_view;
    //! \brief Display messages
    sf::Text m_text;
    //! \brief Selector of menu
    size_t m_cursor = 0u;
};

#endif
