//=============================================================================
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
//=============================================================================

#include "Application/GUIMainMenu.hpp"
#include "Application/GUILoadSimulMenu.hpp"
#include "Renderer/FontManager.hpp"
#include "Common/FileSystem.hpp"
#include "Common/Prolog.hpp"
#include "project_info.hpp" // Generated by the Makefile
#include "MyLogger/Logger.hpp"

//-----------------------------------------------------------------------------
//! \file Entry point of the car simulation application. Check the command line
//! and if no simulation file is providing start a "hello simulation" demo.
//-----------------------------------------------------------------------------

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 1024

// -----------------------------------------------------------------------------
static void display_project_info()
{
    std::cout << "Welcome to the " << project::info::project_name << " project"
              << " version " << project::info::major_version
              << '.' << project::info::minor_version
              << " compiled in "
              << (project::info::mode == project::info::debug ? "debug" : "release")
              << " mode " << std::endl
              << "Your code was git cloned on branch " << project::info::git_branch
              << " SHA1 " << project::info::git_sha1 << std::endl
              << "Project searc path: " << Path::instance().toString() << std::endl
              << "Log file: " << project::info::log_path << std::endl
              << std::endl;
}

// -----------------------------------------------------------------------------
static void init_config()
{
    // Define path for searching resources
#ifdef __APPLE__
    Path::instance().add(osx_get_resources_dir("data"));
#else
    Path::instance().add("data").add(project::info::data_path);
#endif
    CONFIG_LOG(mylogger::project::Info(
        project::info::mode == project::info::debug,
        project::info::application_name.c_str(),
        project::info::major_version,
        project::info::minor_version,
        project::info::git_branch.c_str(),
        project::info::git_sha1.c_str(),
        project::info::data_path.c_str(),
        project::info::tmp_path.c_str(),
        project::info::log_name.c_str(),
        project::info::log_path.c_str()
    ));
    LOGI("Search path: %s", project::info::data_path.c_str());
}

// -----------------------------------------------------------------------------
static int start_highway(int argc, char* const argv[])
{
    // FIXME: find a better solution.
    // Initialize the database of blueprints.
    BluePrints::init();

    // Load fonts
    if (!FontManager::instance().load("main font", "font.ttf"))
    {
        LOGA("Failed init the application %s", argv[0]);
        return EXIT_FAILURE;
    }

    // SFML application
    Application app(WINDOW_WIDTH, WINDOW_HEIGHT,
                    "Highway: Open-source simulator for autonomous driving "
                    "research");

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
        if (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")
        {
            // Display usage
            std::cout << argv[0] << "[scenario file]" << std::endl;
            return EXIT_SUCCESS;
        }
        else if (fs::exists(argv[1]))
        {
            LOGI("Started %s with scenario %s", argv[0], argv[1]);
            app.push<GUIMainMenu>("GUIMainMenu");
            app.push<GUISimulation>("GUISimulation", argv[1]);
        }
        else
        {
            std::cerr << "Fatal: the scenario file '" << argv[1]
                      << "' does not exist"
                      << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Infinite loop managing the GUI
    app.loop();

    return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    init_config();
    display_project_info();
    test_prolog(argc, argv);

    try
    {
        return start_highway(argc, argv);
    }
    catch (std::string const& msg)
    {
        LOGC("%s", msg.c_str());
        std::cerr << "Caught exception: " << msg << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::exception const& e)
    {
        LOGC("%s", e.what());
        std::cerr << "Caught exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
