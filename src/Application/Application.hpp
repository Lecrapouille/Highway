// 2021 -- 2022 Quentin Quadrat quentin.quadrat@gmail.com
//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <https://unlicense.org>

#ifndef APPLICATION_HPP
#  define APPLICATION_HPP

// *****************************************************************************
//! \file This file is an adaptation of the following code source:
//! https://www.binpress.com/tutorial/creating-a-city-building-game-with-sfml/137
//! The adaptation is made for the usage of this particular project.
// *****************************************************************************

#  include <SFML/Graphics.hpp>
#  include <SFML/System.hpp>
#  include <stack>
#  include <cassert>

// *****************************************************************************
//! \brief Manage a stack of GUI instances, which shall deriving from the
//! interface Application::GUI class. Since we desire to manage a very simple
//! application, GUIs are simply pushed and poped in a stack and only the GUI on
//! the top of the stack is active, reacts to IO events (mouse, keyboard ...)
//! and is rendered by the SFML library. Others GUIs in the stack are inactive
//! until they reached the top position in the stack. Therefore a GUI pushing a
//! child GUI in the stack will be in pause until the child GUI is closed.
//! Finally, note that poped GUI have not their memory released: the stack does
//! not hold GUIs but just refer them by raw pointer.
// *****************************************************************************
class Application
{
public:

    // *************************************************************************
    //! \brief Interface class for drawing a GUI and handling mouse and keyboard
    //! events. The application is not released by this class and shall be
    //! destroy after the destruction of this class.
    //!
    //! \pre This class is dependent from the SFML library.
    // *************************************************************************
    class GUI
    {
        //! \brief Access to private methods
        friend class Application;

    public:

        //----------------------------------------------------------------------
        //! \brief Default constructor. No actions are made except initializing
        //! internal states.
        //! \param[in] name: Name of the GUI.
        //! \param[inout] renderer: SFML renderer.
        //----------------------------------------------------------------------
        GUI(const char* name, sf::RenderWindow& render)
            : bgColor(0, 0, 100, 255), m_render(render), m_name(name)
        {}

        //----------------------------------------------------------------------
        //! \brief Needed because of virtual methods.
        //----------------------------------------------------------------------
        virtual ~GUI() = default;

        //----------------------------------------------------------------------
        //! \brief Return the SFML renderer.
        //----------------------------------------------------------------------
        inline sf::RenderWindow& renderer()
        {
            return m_render;
        }

        //----------------------------------------------------------------------
        //! \brief Return then GUI name (debug purpose)
        //----------------------------------------------------------------------
        inline std::string const& name() const
        {
            return m_name;
        }

    private:

        //----------------------------------------------------------------------
        //! \brief Internal methods called by the Application class when the GUI
        //! is pushed in the stack.
        //----------------------------------------------------------------------
        virtual void activate() = 0;

        //----------------------------------------------------------------------
        //! \brief Internal methods called by the Application class when the GUI
        //! is poped from the stack.
        //----------------------------------------------------------------------
        virtual void deactivate() = 0;

        //----------------------------------------------------------------------
        //! \brief Internal methods called by the Application class to know if
        //! the GUI has finished and shall be poped.
        //----------------------------------------------------------------------
        virtual bool isRunning() = 0;

        //----------------------------------------------------------------------
        //! \brief Internal methods called by the Application class when the GUI
        //! has to be rendered.
        //----------------------------------------------------------------------
        virtual void draw() = 0;

        //----------------------------------------------------------------------
        //! \brief Internal methods called by the Application class when the GUI
        //! has to be updated.
        //! \param[in] dt the number of seconds spent this the previous call
        //! (delta time).
        //----------------------------------------------------------------------
        virtual void update(const float dt) = 0;

        //----------------------------------------------------------------------
        //! \brief Internal methods called by the Application class when the GUI
        //! has to manage IO events (mouse, keyboard, ...).
        //----------------------------------------------------------------------
        virtual void handleInput() = 0;

    public:

        //! \brief the background color
        sf::Color bgColor;

    protected:

        //! \brief the FSML renderer
        sf::RenderWindow& m_render;

    private:

        //! \brief the GUI name.
        std::string m_name;
    };

public:

    //--------------------------------------------------------------------------
    //! \brief Create a SFML window with an empty stack of GUI.
    //! \param[in] width: the initial width dimension of the window [pixel].
    //! \param[in] height: the initial height dimension of the window [pixel].
    //! \param[in] title: window title.
    //--------------------------------------------------------------------------
    Application(uint32_t const width, uint32_t const height, std::string const& title)
    {
        m_renderer.create(sf::VideoMode(width, height), title);
        m_renderer.setFramerateLimit(120);
    }

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
    //! \brief Push a new GUI which will be draw by SFML.
    //! \note the GUI will not be released when poped.
    //--------------------------------------------------------------------------
    inline void push(Application::GUI& gui)
    {
        m_guis.push(&gui);
        gui.activate();
    }

    //--------------------------------------------------------------------------
    //! \brief Drop the current GUI. The new GUI on the top of the stack will be
    //! active and draw by SFML.
    //! \note the poped GUI is not released.
    //! \pre The stack shall have at least one GUI.
    //--------------------------------------------------------------------------
    inline void pop()
    {
        assert(!m_guis.empty());
        m_guis.top()->deactivate();
        m_guis.pop();
    }

    //--------------------------------------------------------------------------
    //! \brief Get the GUI placed on the top of the stack.
    //! \return the address of the GUI (nullptr is the stack is empty).
    //--------------------------------------------------------------------------
    inline Application::GUI* peek()
    {
        return m_guis.empty() ? nullptr : m_guis.top();
    }

    //--------------------------------------------------------------------------
    //! \brief Push a new GUI on the top of the stack and start a loop for
    //! managing its draw and IO events. When the GUI is closed it will be drop
    //! from the stack.
    //!
    //! \note this method will call Application::GUI's private pure virtual
    //! methods.
    //--------------------------------------------------------------------------
    void loop(Application::GUI& starting_gui)
    {
        sf::Clock clock;
        Application::GUI* gui;

        push(starting_gui);
        do
        {
            float dt = clock.restart().asSeconds();
            gui = peek();
            assert(gui != nullptr);
            m_renderer.clear(gui->bgColor);
            gui->handleInput();
            gui->update(dt);
            gui->draw();
            m_renderer.display();
        }
        while (gui->isRunning());
        pop();
    }

    //--------------------------------------------------------------------------
    //! \brief Pop the GUI on the top of the stack and start a loop for
    //! managing its draw and IO events. When the GUI is closed it will be drop
    //! from the stack.
    //! \pre the stack shall contain at least one GUI.
    //--------------------------------------------------------------------------
    inline void loop()
    {
        loop(*peek());
    }

    //--------------------------------------------------------------------------
    //! \brief Return the SFML renderer needed to paint SFML shapes
    //! (rectangles, circles ...)
    //--------------------------------------------------------------------------
    inline sf::RenderWindow& renderer()
    {
        return m_renderer;
    }

private:

    //! \brief Stack of GUIs.
    std::stack<Application::GUI*> m_guis;
    //! \brief SFML renderer.
    sf::RenderWindow m_renderer;
};

#endif
