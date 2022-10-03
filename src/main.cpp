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

#include "Application/GUIMainMenu.hpp"
#include "Application/GUILoadSimulMenu.hpp"
#include "Renderer/FontManager.hpp"
#include "Common/FileSystem.hpp"

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
        // FIXME: find a better solution.
        // Initialize the database of blueprints.
        BluePrints::init();

        // Load fonts
        if (!FontManager::instance().load("main font", "font.ttf"))
            return EXIT_FAILURE;

        // SFML application
        Application app(WINDOW_WIDTH, WINDOW_HEIGHT, "Highway: Open-source "
                        "simulator for autonomous driving research");

        // No argument: load an ultra basic simulation set from
        // src/Simulator/Demo.cpp.
        if (argc == 1)
        {
            app.push<GUIMainMenu>("GUIMainMenu");
        }
        // Single argument: load the shared library file passed by command line.
        // The shared library is holding functions for creating the simulation.
        // See Scenarios/API.hpp for more information.
        else
        {
            if (fs::exists(argv[1]))
            {
                app.push<GUIMainMenu>("GUIMainMenu");
                app.push<GUISimulation>("GUISimulation", argv[1]);
            }
            else
            {
                std::cerr << "Fatal: the scenario file '" << argv[1] << "' does not exist"
                          << std::endl;
                return EXIT_FAILURE;
            }
        }

        // Infinite loop managing the GUI
        app.loop();
    }
    catch (std::string const& msg)
    {
        std::cerr << "Fatal: " << msg << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::exception const& e)
    {
        std::cerr << "Fatal: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
