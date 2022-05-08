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
//! https://www.binpress.com/creating-city-building-game-with-sfml-state-manager
//! The adaptation is made for the usage of this particular project.
// *****************************************************************************

#  include <SFML/Graphics.hpp>
#  include <SFML/System.hpp>
#  include <stack>
#  include <cassert>
#  include <iostream>
#  include <memory>

// *****************************************************************************

//! \brief Manage a stack of GUI instances (GUI which shall derive from the
//! interface Application::GUI class). Since we desire to manage a very simple
//! application, GUIs are simply pushed and poped in a stack (\c m_stack) and
//! only the GUI on the top of the stack is active, reacts to IO events (mouse,
//! keyboard ...)  and are rendered by the SFML library. Others stacked GUIs are
//! inactive until they reached the top position in the stack where they become
//! active.  Therefore a GUI pushing a child GUI in the stack will be in pause
//! until the child GUI is poped off the stack.

//! Finally, note when a GUI is puash it does not have not their memory created
//! (and when poped they do not have their memory released). Indeed, the stack
//! holds and refers GUIs fromraw pointers. GUI are in fact created can be
//! stored into a separate internal container (\c m_guis created through the \c
//! create() method and get from the \c gui() method).

// *****************************************************************************
class Application
{
public:

    // *************************************************************************
    //! \brief Interface class for drawing a GUI and handling mouse and keyboard
    //! events. This instance knows the Application instance owning it.
    //! \pre This class is dependent from the SFML library.
    // *************************************************************************
    class GUI
    {
        //! \brief Access to private methods
        friend class Application;

    public:

        //----------------------------------------------------------------------
        //! \brief Default constructor. No actions are made except initializing
        //! internal states with values passed as parameters.
        //! \param[in] application: the instance owning GUIs.
        //! \param[in] name: Name of the GUI. Used as key for Application to
        //!   find it back.
        //! \param[in] color: Background color.
        //----------------------------------------------------------------------
        GUI(Application& application, const char* name, sf::Color const& color
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
        //! \brief Return then GUI name (as search key or for debug purpose).
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
            return m_renderer.setTitle(name);
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
        virtual bool running() const = 0;

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
        sf::Color background_color;

    protected:

        //! \brief The application owning this GUI instance
        Application& m_application;
        //! \brief the FSML renderer for drawing things.
        sf::RenderWindow& m_renderer;

    private:

        //! \brief the GUI name (used as key search or for debug purpose).
        std::string m_name;
    };

public:

    //--------------------------------------------------------------------------
    //! \brief Create a SFML window with an empty stack of GUI.
    //! \param[in] width: the initial width dimension of the window [pixel].
    //! \param[in] height: the initial height dimension of the window [pixel].
    //! \param[in] title: window title.
    //--------------------------------------------------------------------------
    Application(uint32_t const width, uint32_t const height, std::string const& title);

    //--------------------------------------------------------------------------
    //! \brief
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

    //----------------------------------------------------------------------
    //! \brief Create a new GUI whih is not stored in the GUI stack.
    //! \param[in] name he GUI name used as key search or for debug purpose.
    //! \return the reference to the created GUI and stored internally.
    //----------------------------------------------------------------------
    template<class GUI>
    GUI& create(const char* name)
    {
        m_guis.push_back(std::make_unique<GUI>(*this, name));
        return *reinterpret_cast<GUI*>(m_guis.back().get());
    }

    //----------------------------------------------------------------------
    //! \brief Search and return the given GUI name and type.
    //! \param[in] name he GUI name used as key search.
    //! \return the reference to the created GUI and stored internally.
    //! \note if the GUI is not found a new one is created.
    //----------------------------------------------------------------------
    template<class GUI>
    GUI& gui(const char* name)
    {
        for (auto& it: m_guis)
        {
            if (it->name() != name)
                continue;

            GUI* gui = dynamic_cast<GUI*>(it.get());
            if (gui != nullptr)
                return *gui;
        }
        return create<GUI>(name);
    }

    //--------------------------------------------------------------------------
    //! \brief Push a new GUI which will be draw by SFML.
    //! \note the GUI will not be released when poped.
    //--------------------------------------------------------------------------
    void push(Application::GUI& gui);

    //--------------------------------------------------------------------------
    //! \brief Drop the current GUI. The new GUI on the top of the stack will be
    //! active and draw by SFML.
    //! \note the poped GUI is not released.
    //! \pre The stack shall have at least one GUI.
    //--------------------------------------------------------------------------
    void pop();

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
    //!
    //! \note this method will call Application::GUI's private pure virtual
    //! methods.
    //--------------------------------------------------------------------------
    void loop(Application::GUI& starting_gui);

    //--------------------------------------------------------------------------
    //! \brief Return the SFML renderer needed to paint SFML shapes
    //! (rectangles, circles ...)
    //--------------------------------------------------------------------------
    inline sf::RenderWindow& renderer()
    {
        return m_renderer;
    }

    //--------------------------------------------------------------------------
    //! \brief Store a new font.
    //! \param[in] name: the internal name of the font.
    //! \param[in] ttf: the filename of the font. It will be searched in the
    //! following oreder: ttf, ./data/., DATADIR
    //! \return true if the font has been loaded with success.
    //--------------------------------------------------------------------------
    bool font(const char* name, const char* ttf);

    //--------------------------------------------------------------------------
    //! \brief Get the font given its internal name.
    //! \return the reference to the font if present.
    //! \throw if the font was not found.
    //--------------------------------------------------------------------------
    sf::Font& font(const char* name);

private:

    void loop();

private:

    //! \brief Current activate gui
    Application::GUI* m_gui = nullptr;
    //! \brief List of GUIs.
    std::vector<std::unique_ptr<Application::GUI>> m_guis;
    //! \brief Stack of GUIs.
    std::stack<Application::GUI*> m_stack;
    //! \brief SFML renderer.
    sf::RenderWindow m_renderer;
    //! \brief List of fonts
    std::map<std::string, std::unique_ptr<sf::Font>> m_fonts;
};

#endif
