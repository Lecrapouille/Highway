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

#ifndef GUI_MAIN_MENU_HPP
#  define GUI_MAIN_MENU_HPP

#  include "Application/Application.hpp"
#  include "Application/GUISimulation.hpp"
#  include <atomic>

// ****************************************************************************
//! \brief Concrete SFML Application::GUI managing the GUI of the main menu.
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
    virtual void draw() override;

    //-------------------------------------------------------------------------
    //! \brief Update the simulation.
    //-------------------------------------------------------------------------
    virtual void update(const float dt) override;

    //-------------------------------------------------------------------------
    //! \brief Manage mouse and keyboard events.
    //-------------------------------------------------------------------------
    virtual void handleInput() override;

    //-------------------------------------------------------------------------
    //! \brief Return true if GUI is alive.
    //-------------------------------------------------------------------------
    virtual bool running() const override
    {
        return m_running;
    }

    //-------------------------------------------------------------------------
    //! \brief Pause the simulation.
    //-------------------------------------------------------------------------
    virtual void activate() override;

    //-------------------------------------------------------------------------
    //! \brief Unpause the simulation.
    //-------------------------------------------------------------------------
    virtual void deactivate() override;

    //-------------------------------------------------------------------------
    //! \brief Create the simulation.
    //-------------------------------------------------------------------------
    virtual void create() override;

    //-------------------------------------------------------------------------
    //! \brief Release the simulation.
    //-------------------------------------------------------------------------
    virtual void release() override;

private:

    //! \brief View
    sf::View m_view;
    //! \brief Display messages
    sf::Text m_text;
    //! \brief Halting the GUI ?
    std::atomic<bool> m_running{true};
};

#endif
