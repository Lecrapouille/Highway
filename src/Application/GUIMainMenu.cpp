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

#include "Application/GUIMainMenu.hpp"
#include "Renderer/FontManager.hpp"

//------------------------------------------------------------------------------
GUIMainMenu::GUIMainMenu(Application& application, const char* name)
    : Application::GUI(application, name, sf::Color::White)
{
    m_view = renderer().getDefaultView();

    m_text.setFont(FontManager::instance().font("main font"));
    m_text.setString("Press space to start simulation");
    m_text.setCharacterSize(24);
    m_text.setFillColor(sf::Color::Red);
    m_text.setStyle(sf::Text::Bold | sf::Text::Underlined);
}

//------------------------------------------------------------------------------
void GUIMainMenu::activate()
{
    m_renderer.setView(m_view);
}

//------------------------------------------------------------------------------
void GUIMainMenu::deactivate()
{
    // FIXME do not push on stack from here else this will create inifinite loop
    // if (!m_renderer.close()) {
    //   m_application.push(m_application.gui<GUISimulation>("GUISimulation"));
    // }
}

//------------------------------------------------------------------------------
void GUIMainMenu::create()
{}

//------------------------------------------------------------------------------
void GUIMainMenu::release()
{}

//------------------------------------------------------------------------------
void GUIMainMenu::handleInput()
{
    sf::Event event;

    while (/*m_running &&*/ m_renderer.pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            //m_running = false;
            m_renderer.close();
            break;
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Escape)
            {
                m_renderer.close();
                //m_running = false;
            }
            else if (event.key.code == sf::Keyboard::Space)
            {
                m_application.push(m_application.gui<GUISimulation>("GUISimulation"));
            }
            break;
        default:
            break;
        }
    }
}

//------------------------------------------------------------------------------
void GUIMainMenu::update(const float dt)
{}

//------------------------------------------------------------------------------
void GUIMainMenu::draw()
{
    renderer().draw(m_text);
}
