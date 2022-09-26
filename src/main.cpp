//=============================================================================
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
//=============================================================================

#include "Simulator/Demo.hpp"
#include "Application/GUIMainMenu.hpp"
#include "Application/GUISimulation.hpp"
#include "Application/GUILoadSimulMenu.hpp"
#include "Renderer/FontManager.hpp"

//-----------------------------------------------------------------------------
//! \file Entry point of the car simulation application. Check the command line
//! and if no simulation file is providing start a "hello simulation" demo.
//-----------------------------------------------------------------------------

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024

// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    try
    {
        // Load fonts
        if (!FontManager::instance().load("main font", "font.ttf"))
            return EXIT_FAILURE;

        // SFML application
        Application app(WINDOW_WIDTH, WINDOW_HEIGHT, "Highway: Open-source simulator for autonomous driving research");

        // Create the simulation GUI and get its simulator.
        Simulator& simulator = app.gui<GUISimulation>("GUISimulation").simulator;

        // No argument: load an ultra basic simulation (see static functions upper).
        if (argc == 1)
        {
            simple_simulation_demo(simulator);
        }
        // Single argument: load the shared library file, passed by command line,
        // shared library holding scenario functions for creating the simulation.
        else // TODO add a real command line parser
        {
            if (!simulator.load(argv[1]))
            {
                std::cerr << "Fatal: failed loading a simulation file. Reason: "
                          << simulator.error() << ". Aborting ..."
                          << std::endl;
                return EXIT_FAILURE;
            }
        }

        // Run the current GUI.
        GUIMainMenu& gui_menu = app.gui<GUIMainMenu>("GUIMainMenu");
        app.loop(gui_menu);
    }
    catch (std::string const& msg)
    {
        std::cerr << "Fatal: " << msg << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
