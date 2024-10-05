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

// *****************************************************************************
//! \file This file is an adaptation of the following code source:
//! https://www.binpress.com/creating-city-building-game-with-sfml-state-manager
//! The adaptation is made for the usage of this particular project.
// *****************************************************************************

#  include "MyLogger/Logger.hpp"
#  include "Core/Math/Units.hpp"
#  include <SFML/Graphics.hpp>
#  include <SFML/System.hpp>
#  include <stack>
#  include <map>
#  include <cassert>
#  include <memory>

// *****************************************************************************
//! \brief Manage a stack of GUI instances (GUI which shall derive from the
//! interface Application::GUI class). Since we desire to manage a very simple
//! application, GUIs are simply pushed and poped in a stack (\c m_stack) and
//! only the GUI on the top of the stack is active, reacts to IO events (mouse,
//! keyboard ...) and is rendered by the SFML library. Others deeper stacked
//! GUIs are inactive until they reached the top position in the stack where
//! they become active. Therefore an active GUI pushing a child GUI in the stack
//! will be in pause until the child GUI is poped off the stack (for example
//! when pushing a submenu). This mimic Android behavior.
//!
//! Finally, note that when a GUI is pushed or poped, it does not have its
//! memory created or released. Indeed, the stack holds raw pointers on GUI
//! instances because when a GUI is created (through the \c create() method) it
//! is stored into a separate internal container (\c m_guis) and pointer get
//! from the \c gui() method).
// *****************************************************************************
class Application
{
public:

    // *************************************************************************
    //! \brief Interface class for drawing a Graphical User Interface (GUI) and
    //! handling mouse and keyboard events. This instance knows the Application
    //! instance owning it.
    //! \pre This class is dependent from the SFML library but could be totally
    //! abstract.
    // *************************************************************************
    class GUI
    {
        //! \brief Access to private methods
        friend class Application;

    public:

        //----------------------------------------------------------------------
        //! \brief Default constructor. No actions are made except initializing
        //! internal states with values passed as parameters.
        //! \param[in] application: the instance owning this GUI.
        //! \param[in] name: Name of the GUI, used as key for Application to
        //!   find it back.
        //! \param[in] color: Background color.
        //----------------------------------------------------------------------
        GUI(Application& application, std::string const& name, sf::Color const& color
            = sf::Color::White);

        //----------------------------------------------------------------------
        //! \brief Needed because of virtual methods.
        //----------------------------------------------------------------------
        virtual ~GUI() = default;

        //----------------------------------------------------------------------
        //! \brief Return the SFML renderer.
        //----------------------------------------------------------------------
        inline sf::RenderWindow& renderer()
        {
            return m_renderer;
        }

        //----------------------------------------------------------------------
        //! \brief Return then GUI name used either as search key or for debug
        //! purpose.
        //----------------------------------------------------------------------
        inline std::string const& name() const
        {
            return m_name;
        }

        //----------------------------------------------------------------------
        //! \brief Change the windows title.
        //----------------------------------------------------------------------
        inline void title(std::string const& name)
        {
            m_renderer.setTitle(name);
        }

        //----------------------------------------------------------------------
        //! \brief Close the GUI: it will be poped and if it was the last GUI
        //! the application will close.
        //----------------------------------------------------------------------
        inline void close()
        {
            m_closing = true;
        }

        //----------------------------------------------------------------------
        //! \brief Close the GUI: it will be poped and if it was the last GUI
        //! the application will close.
        //----------------------------------------------------------------------
        inline void halt()
        {
            m_halting = true;
        }

    private:

        //----------------------------------------------------------------------
        //! \brief Internal methods called by the Application class when the GUI
        //! is pushed in the stack.
        //----------------------------------------------------------------------
        virtual void onCreate() = 0;

        //----------------------------------------------------------------------
        //! \brief Internal methods called by the Application class when the GUI
        //! is poped from the stack.
        //----------------------------------------------------------------------
        virtual void onRelease() = 0;

        //----------------------------------------------------------------------
        //! \brief Internal methods called by the Application class when the GUI
        //! is no longer on the top of the stack.
        //----------------------------------------------------------------------
        virtual void onDeactivate() = 0;

        //----------------------------------------------------------------------
        //! \brief Internal methods called by the Application class when the GUI
        //! has returned to the top of the stack.
        //----------------------------------------------------------------------
        virtual void onActivate() = 0;

        //----------------------------------------------------------------------
        //! \brief Internal methods called by the Application class when the GUI
        //! has to be rendered.
        //----------------------------------------------------------------------
        virtual void onDraw() = 0;

        //----------------------------------------------------------------------
        //! \brief Internal methods called by the Application class when the GUI
        //! has to be updated.
        //! \param[in] dt the number of seconds spent this the previous call
        //! (delta time).
        //----------------------------------------------------------------------
        virtual void onUpdate(const Second dt) = 0;

        //----------------------------------------------------------------------
        //! \brief Internal methods called by the Application class when the GUI
        //! has to manage IO events (mouse, keyboard, ...).
        //----------------------------------------------------------------------
        virtual void onHandleInput() = 0;

    public:

        //! \brief the background color
        sf::Color background_color;

    protected:

        //! \brief The application owning this GUI instance
        Application& m_application;
        //! \brief the FSML renderer for drawing things.
        sf::RenderWindow& m_renderer;

    private:

        //! \brief the GUI name (used as key search or for debug purpose).
        std::string m_name;
        //! \brief Closing the GUI ?
        bool m_closing = false;
        //! \brief Halting the application ?
        bool m_halting = false;
    }; // class GUI

public:

    //--------------------------------------------------------------------------
    //! \brief Create a SFML window with an empty stack of GUIs.
    //! \param[in] width: the initial width dimension of the window [pixel].
    //! \param[in] height: the initial height dimension of the window [pixel].
    //! \param[in] title: window title.
    //--------------------------------------------------------------------------
    Application(uint32_t const width, uint32_t const height, std::string const& title);

    //--------------------------------------------------------------------------
    //! \brief Clear the stack of GUIs and close the renderer.
    //--------------------------------------------------------------------------
    ~Application();

    //--------------------------------------------------------------------------
    //! \brief Return the window width [pixel].
    //--------------------------------------------------------------------------
    inline uint32_t width() const
    {
        return m_renderer.getSize().x;
    }

    //--------------------------------------------------------------------------
    //! \brief Return the window height [pixel].
    //--------------------------------------------------------------------------
    inline uint32_t height() const
    {
        return m_renderer.getSize().y;
    }

    //--------------------------------------------------------------------------
    //! \brief Take a screenshot of the game and save it as PNG to the given path.
    //--------------------------------------------------------------------------
    bool screenshot(std::string const& screenshot_path) const;

#if 0
    //--------------------------------------------------------------------------
    //! \brief Create a new GUI on its internal container. The GUI is not pushed
    //! in the GUI stack (it is the developper decision to push or pop GUIs). If
    //! a GUI with the same name is already present it is replaced by the new
    //! one.
    //! \param[in] name the GUI name used as key search or for debug purpose.
    //! \return the reference to the created GUI and stored internally (not
    //! deferenced by insertion).
    //--------------------------------------------------------------------------
    template<class GUI>
    GUI& create(std::string const& name)
    {
        m_guis[name] = std::make_unique<GUI>(*this, name);
        return *reinterpret_cast<GUI*>(m_guis[name].get());
    }
#endif

    //--------------------------------------------------------------------------
    //! \brief Search and return the given GUI name and type.
    //! \param[in] name he GUI name used as key search.
    //! \return the reference to the created GUI and stored internally.
    //! \note if the GUI is not found a new one is created.
    //--------------------------------------------------------------------------
    template<class GUI, typename ...ArgsT>
    GUI& gui(std::string const& name, ArgsT&&... args)
    {
        //auto const& it = m_guis.find(name); // TBD: use typeid(T).hash_code() instead of name
        //if (it != m_guis.end())
        //{
        //    // FIXME: if we do not remove existing GUI the arguments are not be passed
        //    // and therefore they are not used.
        //    // https://github.com/Lecrapouille/Highway/issues/2
        //    //return *dynamic_cast<GUI*>(it->second.get());
        //    m_guis.erase(it);
        //}
        m_guis[name] = std::make_unique<GUI>(*this, name, std::forward<ArgsT>(args)...);
        return *reinterpret_cast<GUI*>(m_guis[name].get());
    }

    //--------------------------------------------------------------------------
    //! \brief Push a new GUI which will be draw by SFML.
    //! \note the GUI will not be released when poped.
    //--------------------------------------------------------------------------
    void push(Application::GUI& gui);

    //--------------------------------------------------------------------------
    //! \brief
    //--------------------------------------------------------------------------
    template<class GUI, typename ...ArgsT>
    void push(std::string const& name, ArgsT&&... args)
    {
        push(gui<GUI>(name, std::forward<ArgsT>(args)...));
    }

    //--------------------------------------------------------------------------
    //! \brief Drop the current GUI. The new GUI on the top of the stack will be
    //! active and draw by SFML.
    //! \note the poped GUI is not released.
    //! \pre The stack shall have at least one GUI.
    //--------------------------------------------------------------------------
    bool pop();

    //--------------------------------------------------------------------------
    //! \brief Get the GUI placed on the top of the stack.
    //! \return the address of the GUI (nullptr is the stack is empty).
    //--------------------------------------------------------------------------
    inline Application::GUI* peek()
    {
        return m_stack.empty() ? nullptr : m_stack.top();
    }

    //--------------------------------------------------------------------------
    //! \brief Push a new GUI on the top of the stack and start a loop for
    //! managing its draw and IO events. When the GUI is closed it will be drop
    //! from the stack.
    //! \param[inout] starting_gui the initial GUI to start the application with.
    //! \param[in] rate fixed time step. By fault set to 60 Hertz.
    //! \note this method will call Application::GUI's private pure virtual
    //! methods.
    //--------------------------------------------------------------------------
    void loop(Application::GUI& starting_gui, uint8_t rate = 60u);
    void loop(uint8_t rate = 60u);

    //--------------------------------------------------------------------------
    //! \brief Return the SFML renderer needed to paint SFML shapes
    //! (rectangles, circles ...)
    //--------------------------------------------------------------------------
    inline sf::RenderWindow& renderer()
    {
        return m_renderer;
    }

    //--------------------------------------------------------------------------
    //! \brief Show the stack of GUIs.
    //--------------------------------------------------------------------------
    inline void printStack();

private:

    //--------------------------------------------------------------------------
    //! \brief Clear the stack of GUIs and close the renderer.
    //--------------------------------------------------------------------------
    void halt();

    //--------------------------------------------------------------------------
    //! \brief Helper method to show the stack of GUIs.
    //--------------------------------------------------------------------------
    void printStack(std::stack<Application::GUI*>& stack);

private:

    //! \brief List of created GUIs.
    std::map<std::string, std::unique_ptr<Application::GUI>> m_guis;
    //! \brief Stack of GUIs.
    std::stack<Application::GUI*> m_stack;
    //! \brief SFML renderer.
    sf::RenderWindow m_renderer;
};