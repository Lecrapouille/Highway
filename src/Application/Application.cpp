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

#include "Application.hpp"

// -----------------------------------------------------------------------------
Application::GUI::GUI(Application& application, const char* name,
                      sf::Color const& color)
    : background_color(color), m_application(application),
      m_renderer(application.renderer()), m_name(name)
{}

// -----------------------------------------------------------------------------
Application::Application(uint32_t const width, uint32_t const height,
                         std::string const& title)
{
    m_renderer.create(sf::VideoMode(width, height), title);
    m_renderer.setFramerateLimit(120);
}

// -----------------------------------------------------------------------------
Application::~Application()
{
    halt();
}

// -----------------------------------------------------------------------------
void Application::halt()
{
    // Clear the satck of GUIs
    std::stack<Application::GUI*>().swap(m_stack);
    // Stop the SFML renderer
    m_renderer.close();
}

// -----------------------------------------------------------------------------
// FIXME recurrsion ConcreteGUI::onDeactivate() { ConcreteGUI::push(NewGUI); }
void Application::push(Application::GUI& gui)
{
    GUI* current_gui = peek();
    if ((current_gui != nullptr) && (current_gui != &gui))
    {
        std::cout << "Deactivate GUI: " << current_gui->name() << std::endl;
        current_gui->onDeactivate();
    }
    m_stack.push(&gui);
    std::cout << "Create GUI: " << gui.name() << std::endl;
    gui.onCreate();
}

// -----------------------------------------------------------------------------
bool Application::pop()
{
    GUI* gui = peek();
    if (gui == nullptr)
        return false;

    std::cout << "Release GUI: " << gui->name() << std::endl;
    m_stack.pop();
    gui->onRelease();

    gui = peek();
    if (gui != nullptr)
    {
        std::cout << "Activate GUI: " << gui->name() << std::endl;
        gui->m_closing = gui->m_halting = false;
        gui->onActivate();
    }

    return true;
}

// -----------------------------------------------------------------------------
void Application::loop(Application::GUI& starting_gui, uint8_t const rate)
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    const sf::Time time_per_frame = sf::seconds(1.0f / float(rate));

    push(starting_gui);
    while (m_renderer.isOpen())
    {
        GUI* gui = peek();
        if (gui == nullptr)
            return ;

        // Process events at fixed time steps
        timeSinceLastUpdate += clock.restart();
        while (timeSinceLastUpdate > time_per_frame)
        {
            timeSinceLastUpdate -= time_per_frame;
            gui->onHandleInput();
            gui->onUpdate(time_per_frame.asSeconds());
        }

        // Rendering
        m_renderer.clear(gui->background_color);
        gui->onDraw();
        m_renderer.display();

        // Halt the application
        if (gui->m_halting)
        {
            halt();
        }
        // Close the current GUI
        else if (gui->m_closing)
        {
            if (!pop())
            {
                m_renderer.close();
            }
        }
    }
}
